#include "AbxrQRService.h"
#include "Async/Async.h"
#include "Types/AbxrLog.h"

#if PLATFORM_ANDROID
FAbxrQRService* FAbxrQRService::ActiveInstance = nullptr;
static TSet<FString> RejectedCodes;
#endif

FAbxrQRService::FAbxrQRService() { }

FAbxrQRService::~FAbxrQRService()
{
	if (bIsInitialized) Shutdown();
}

void FAbxrQRService::Initialize()
{
	if (bIsInitialized) return;
	bIsInitialized = true;
#if PLATFORM_ANDROID
	ActiveInstance = this;
	RegisterNativeMethods();
	InitJava();
#endif
}

void FAbxrQRService::Shutdown()
{
	if (!bIsInitialized) return;
	bIsInitialized = false;
	bIsScanning = false;
#if PLATFORM_ANDROID
	CleanupJNI();
	ActiveInstance = nullptr;
#endif
}

bool FAbxrQRService::IsSupported()
{
#if PLATFORM_ANDROID
	FString DeviceModel = FAndroidMisc::GetDeviceModel().ToLower();
	return DeviceModel.Contains(TEXT("quest 3"));
#else
	return false;
#endif
}

void FAbxrQRService::StartScan()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("AbxrQRService::StartScan — service not initialized"));
		return;
	}

	if (bIsScanning)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("AbxrQRService::StartScan — scan already in progress, ignoring"));
		return;
	}

#if PLATFORM_ANDROID
	bIsScanning = true;
	RejectedCodes.Reset();
	if (!QRScannerObject)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::StartScan — Java scanner object not available"));
		bIsScanning = false;
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::StartScan — failed to get JNI env"));
		bIsScanning = false;
		return;
	}

	jobject Activity = FAndroidApplication::GetGameActivityThis();
	if (!Activity)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::StartScan — failed to get Activity"));
		bIsScanning = false;
		return;
	}

	jmethodID StartMethod = Env->GetMethodID(QRScannerClass, "startScan", "(Landroid/app/Activity;)V");
	if (!StartMethod)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::StartScan — could not find startScan method"));
		bIsScanning = false;
		return;
	}

	Env->CallVoidMethod(QRScannerObject, StartMethod, Activity);

	if (Env->ExceptionCheck())
	{
		Env->ExceptionDescribe();
		Env->ExceptionClear();
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::StartScan — exception thrown by Java"));
		bIsScanning = false;
		return;
	}
#endif
}

void FAbxrQRService::StopScan()
{
	if (!bIsScanning) return;

#if PLATFORM_ANDROID
	if (QRScannerObject)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();
		if (Env)
		{
			jmethodID StopMethod = Env->GetMethodID(QRScannerClass, "stopScan", "()V");
			if (StopMethod)
			{
				Env->CallVoidMethod(QRScannerObject, StopMethod);
				if (Env->ExceptionCheck()) { Env->ExceptionClear(); }
			}
		}
	}
#endif

	bIsScanning = false;
}

#if PLATFORM_ANDROID
void FAbxrQRService::InitJava()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::InitJava — no JNI env"));
		return;
	}

	jclass LocalClass = FAndroidApplication::FindJavaClass("com.abxr.qr.AbxrQRScanner");
	if (!LocalClass)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::InitJava — could not find AbxrQRScanner class"));
		return;
	}

	QRScannerClass = (jclass)Env->NewGlobalRef(LocalClass);
	Env->DeleteLocalRef(LocalClass);

	jmethodID Constructor = Env->GetMethodID(QRScannerClass, "<init>", "()V");
	if (!Constructor)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::InitJava — could not find AbxrQRScanner constructor"));
		return;
	}

	jobject LocalObj = Env->NewObject(QRScannerClass, Constructor);
	if (!LocalObj)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::InitJava — failed to construct AbxrQRScanner"));
		return;
	}

	QRScannerObject = Env->NewGlobalRef(LocalObj);
	Env->DeleteLocalRef(LocalObj);
}

void FAbxrQRService::RegisterNativeMethods()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::RegisterNativeMethods — no JNI env"));
		return;
	}

	jclass CallbackClass = FAndroidApplication::FindJavaClass("com.abxr.qr.AbxrQRScannerCallback");
	if (!CallbackClass)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::RegisterNativeMethods — could not find AbxrQRScannerCallback class"));
		return;
	}

	JNINativeMethod NativeMethods[] =
	{
		{ "nativeOnScanned", "(Ljava/lang/String;)V", (void*)&FAbxrQRService::NativeOnScanned },
		{ "nativeOnFailed", "(Ljava/lang/String;)V", (void*)&FAbxrQRService::NativeOnFailed },
		{ "nativeOnCancelled", "()V", (void*)&FAbxrQRService::NativeOnCancelled },
	};

	const int Result = Env->RegisterNatives(CallbackClass, NativeMethods, 3);
	if (Result < 0)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("AbxrQRService::RegisterNativeMethods — RegisterNatives failed"));
	}
}

void FAbxrQRService::CleanupJNI()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (Env)
	{
		if (QRScannerObject)
		{
			jmethodID ReleaseMethod = Env->GetMethodID(QRScannerClass, "release", "()V");
			if (ReleaseMethod)
			{
				Env->CallVoidMethod(QRScannerObject, ReleaseMethod);
				if (Env->ExceptionCheck()) { Env->ExceptionClear(); }
			}
			Env->DeleteGlobalRef(QRScannerObject);
			QRScannerObject = nullptr;
		}
		if (QRScannerClass)
		{
			Env->DeleteGlobalRef(QRScannerClass);
			QRScannerClass = nullptr;
		}
	}
}

void JNICALL FAbxrQRService::NativeOnScanned(JNIEnv* Env, jclass Class, jstring Result)
{
	const char* Chars = Env->GetStringUTFChars(Result, nullptr);
	FString DecodedText(UTF8_TO_TCHAR(Chars));
	Env->ReleaseStringUTFChars(Result, Chars);
	
	jclass ScannerClass = FAndroidApplication::FindJavaClass("com.abxr.qr.AbxrQRScanner");

	const FString AbxrPrefix = TEXT("ABXR:");
	if (!DecodedText.RemoveFromStart(AbxrPrefix))
	{
		if (!RejectedCodes.Contains(DecodedText))
		{
			RejectedCodes.Add(DecodedText);
			UE_LOG(LogAbxrLib, Warning, TEXT("QR code not ABXR format (got: %s), continuing scan"), *DecodedText);
		}

		// Tell the activity to keep scanning
		jmethodID RejectMethod = Env->GetStaticMethodID(ScannerClass, "rejectScanResult", "()V");
		Env->CallStaticVoidMethod(ScannerClass, RejectMethod);
		return;
	}
	
	jmethodID AcceptMethod = Env->GetStaticMethodID(ScannerClass, "acceptScanResult", "()V");
	Env->CallStaticVoidMethod(ScannerClass, AcceptMethod);

	AsyncTask(ENamedThreads::GameThread, [DecodedText]
	{
		if (ActiveInstance && ActiveInstance->bIsScanning)
		{
			ActiveInstance->bIsScanning = false;
			ActiveInstance->OnQRCodeScanned.Broadcast(DecodedText);
		}
	});
}

void JNICALL FAbxrQRService::NativeOnFailed(JNIEnv* Env, jclass Class, jstring Reason)
{
	const char* Chars = Env->GetStringUTFChars(Reason, nullptr);
	FString ReasonStr(UTF8_TO_TCHAR(Chars));
	Env->ReleaseStringUTFChars(Reason, Chars);

	UE_LOG(LogAbxrLib, Warning, TEXT("AbxrQRService::NativeOnFailed — reason: %s"), *ReasonStr);

	AsyncTask(ENamedThreads::GameThread, []
	{
		if (ActiveInstance && ActiveInstance->bIsScanning)
		{
			ActiveInstance->bIsScanning = false;
			ActiveInstance->OnQRCodeScanned.Broadcast(TEXT(""));
		}
	});
}

void JNICALL FAbxrQRService::NativeOnCancelled(JNIEnv* Env, jclass Class)
{
	AsyncTask(ENamedThreads::GameThread, []
	{
		if (ActiveInstance && ActiveInstance->bIsScanning)
		{
			ActiveInstance->bIsScanning = false;
			ActiveInstance->OnQRCodeScanned.Broadcast(TEXT(""));
		}
	});
}
#endif
