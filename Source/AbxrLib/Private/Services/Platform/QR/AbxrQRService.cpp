#include "Services/Platform/QR/AbxrQRService.h"
#include "Async/Async.h"
#include "Types/AbxrLog.h"
#if PLATFORM_ANDROID
FAbxrQRService* FAbxrQRService::ActiveInstance = nullptr;
bool FAbxrQRService::bNativeMethodsRegistered = false;
#endif

FAbxrQRService::FAbxrQRService()
{
}

FAbxrQRService::~FAbxrQRService()
{
	Shutdown();
}

void FAbxrQRService::Initialize()
{
	if (bIsInitialized) return;
	bIsInitialized = true;

#if PLATFORM_ANDROID
	ActiveInstance = this;
	if (!bNativeMethodsRegistered) bNativeMethodsRegistered = RegisterNativeMethods();
#endif
}

void FAbxrQRService::Shutdown()
{
	if (!bIsInitialized) return;
	StopScan();

#if PLATFORM_ANDROID
	if (ActiveInstance == this) ActiveInstance = nullptr;
	CleanupJNI();
#endif

	bIsInitialized = false;
}

bool FAbxrQRService::IsSupported()
{
#if PLATFORM_ANDROID
	return true;
#else
	return false;
#endif
}

bool FAbxrQRService::StartScan()
{
#if PLATFORM_ANDROID
	if (!bIsInitialized) Initialize();

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: no JNI env"));
		return false;
	}

	jobject Activity = FAndroidApplication::GetGameActivityThis();
	if (!Activity)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: no GameActivity"));
		return false;
	}

	if (!bNativeMethodsRegistered)
	{
		bNativeMethodsRegistered = RegisterNativeMethods();
		if (!bNativeMethodsRegistered)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: native registration failed"));
			return false;
		}
	}

	if (!EnsureScannerInstance()) return false;

	jmethodID StartMethod = Env->GetMethodID(ScannerClass, "startScan", "(Landroid/app/Activity;)V");
	if (!StartMethod)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: missing startScan(Activity)"));
		return false;
	}

	Env->CallVoidMethod(ScannerInstance, StartMethod, Activity);
	if (Env->ExceptionCheck())
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: exception from startScan"));

		jthrowable Exception = Env->ExceptionOccurred();
		Env->ExceptionDescribe();
		Env->ExceptionClear();

		if (Exception)
		{
			jclass ThrowableClass = Env->GetObjectClass(Exception);
			jmethodID ToStringMethod = Env->GetMethodID(ThrowableClass, "toString", "()Ljava/lang/String;");
			jstring Message = (jstring)Env->CallObjectMethod(Exception, ToStringMethod);

			const FString ExceptionText = FJavaHelper::FStringFromLocalRef(Env, Message);
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: startScan exception detail: %s"), *ExceptionText);

			Env->DeleteLocalRef(ThrowableClass);
			Env->DeleteLocalRef(Exception);
		}

		return false;
	}

	bIsScanning = true;
	return true;
#else
	return false;
#endif
}

void FAbxrQRService::StopScan()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (Env && ScannerClass && ScannerInstance)
	{
		jmethodID StopMethod = Env->GetMethodID(ScannerClass, "stopScan", "()V");
		if (StopMethod)
		{
			Env->CallVoidMethod(ScannerInstance, StopMethod);
			if (Env->ExceptionCheck())
			{
				Env->ExceptionDescribe();
				Env->ExceptionClear();
			}
		}
	}
#endif

	bIsScanning = false;
}

#if PLATFORM_ANDROID
bool FAbxrQRService::RegisterNativeMethods()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return false;

	jclass CallbackClass = FAndroidApplication::FindJavaClass("com.abxr.qr.AbxrQRScannerCallback");
	if (!CallbackClass)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: failed to find callback class"));
		return false;
	}

	JNINativeMethod NativeMethods[] =
	{
		{ "nativeOnScanned", "(Ljava/lang/String;)V", (void*)&FAbxrQRService::NativeOnScanned },
		{ "nativeOnFailed", "(Ljava/lang/String;)V", (void*)&FAbxrQRService::NativeOnFailed },
		{ "nativeOnCancelled", "()V", (void*)&FAbxrQRService::NativeOnCancelled },
		{ "nativeOnPreviewFrame", "([BII)V", (void*)&FAbxrQRService::NativeOnPreviewFrame }
	};

	const int Result = Env->RegisterNatives(CallbackClass, NativeMethods, UE_ARRAY_COUNT(NativeMethods));
	if (Result < 0)
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("QR: RegisterNatives failed"));
		return false;
	}

	return true;
}

bool FAbxrQRService::EnsureScannerInstance()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return false;

	if (!ScannerClass)
	{
		ScannerClass = FAndroidApplication::FindJavaClass("com.abxr.qr.AbxrQRScanner");
		if (!ScannerClass)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: failed to find scanner class"));
			return false;
		}
	}

	if (!ScannerInstance)
	{
		jmethodID Constructor = Env->GetMethodID(ScannerClass, "<init>", "()V");
		if (!Constructor)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: missing scanner constructor"));
			return false;
		}

		jobject LocalScanner = Env->NewObject(ScannerClass, Constructor);
		if (!LocalScanner)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: failed to construct scanner"));
			return false;
		}

		ScannerInstance = Env->NewGlobalRef(LocalScanner);
		Env->DeleteLocalRef(LocalScanner);
		if (!ScannerInstance)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("QR: failed to create global scanner ref"));
			return false;
		}
	}

	return true;
}

void FAbxrQRService::CleanupJNI()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return;

	if (ScannerInstance)
	{
		Env->DeleteGlobalRef(ScannerInstance);
		ScannerInstance = nullptr;
	}
}

void JNICALL FAbxrQRService::NativeOnScanned(JNIEnv* Env, jclass Clazz, jstring DecodedText)
{
	if (!ActiveInstance || !DecodedText) return;

	const char* UtfChars = Env->GetStringUTFChars(DecodedText, nullptr);
	const FString Value = UTF8_TO_TCHAR(UtfChars ? UtfChars : "");
	if (UtfChars)
	{
		Env->ReleaseStringUTFChars(DecodedText, UtfChars);
	}

	AsyncTask(ENamedThreads::GameThread, [Value]()
	{
		if (!ActiveInstance) return;
		ActiveInstance->bIsScanning = false;
		ActiveInstance->OnQRCodeScanned.Broadcast(Value);
	});
}

void JNICALL FAbxrQRService::NativeOnFailed(JNIEnv* Env, jclass Clazz, jstring Reason)
{
	if (!ActiveInstance) return;

	FString Value;
	if (Reason)
	{
		const char* UtfChars = Env->GetStringUTFChars(Reason, nullptr);
		Value = UTF8_TO_TCHAR(UtfChars ? UtfChars : "");
		if (UtfChars)
		{
			Env->ReleaseStringUTFChars(Reason, UtfChars);
		}
	}

	AsyncTask(ENamedThreads::GameThread, [Value]()
	{
		if (!ActiveInstance) return;
		ActiveInstance->bIsScanning = false;
		ActiveInstance->OnQRScanFailed.Broadcast(Value);
	});
}

void JNICALL FAbxrQRService::NativeOnCancelled(JNIEnv* Env, jclass Clazz)
{
	if (!ActiveInstance) return;

	AsyncTask(ENamedThreads::GameThread, []()
	{
		if (!ActiveInstance) return;
		ActiveInstance->bIsScanning = false;
		ActiveInstance->OnQRScanCancelled.Broadcast();
	});
}

void JNICALL FAbxrQRService::NativeOnPreviewFrame(JNIEnv* Env, jclass Clazz, jbyteArray RgbaBytes, jint Width, jint Height)
{
	if (!ActiveInstance || !RgbaBytes) return;

	const jsize NumBytes = Env->GetArrayLength(RgbaBytes);
	TArray<uint8> Copy;
	Copy.SetNumUninitialized(NumBytes);
	Env->GetByteArrayRegion(RgbaBytes, 0, NumBytes, reinterpret_cast<jbyte*>(Copy.GetData()));

	AsyncTask(ENamedThreads::GameThread, [Pixels = MoveTemp(Copy), Width = (int32)Width, Height = (int32)Height]() mutable
	{
		if (!ActiveInstance || !ActiveInstance->bIsScanning) return;
		ActiveInstance->OnQRPreviewFrame.Broadcast(Pixels, Width, Height);
	});
}
#endif
