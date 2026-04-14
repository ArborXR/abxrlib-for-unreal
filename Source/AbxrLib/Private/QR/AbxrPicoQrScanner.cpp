#include "QR/AbxrPicoQrScanner.h"

#include "Engine/Texture2D.h"
#include "Misc/ScopeLock.h"
#include "QR/AbxrQrScanCommon.h"
#include "Types/AbxrLog.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJavaEnv.h"
#endif

namespace
{
#if PLATFORM_ANDROID
	struct FAbxrPicoQrBridgeState
	{
		FCriticalSection Mutex;
		TWeakObjectPtr<UAbxrPicoQrScanner> ActiveScanner;
		bool bNativesRegistered = false;

		TArray<uint8> LatestBgraFrame;
		int32 LatestFrameWidth = 0;
		int32 LatestFrameHeight = 0;
		bool bHasNewFrame = false;

		FString LatestPayload;
		bool bHasPayload = false;

		int32 LatestStatusCode = INDEX_NONE;
		FString LatestStatusMessage;
		bool bHasStatus = false;
	};

	FAbxrPicoQrBridgeState GAbxrPicoQrBridgeState;

	FString JStringToFString(JNIEnv* Env, jstring Value)
	{
		if (!Env || !Value) return FString();
		return FJavaHelper::FStringFromLocalRef(Env, Value);
	}

	void JNICALL NativeOnAbxrQrFrame(JNIEnv* Env, jobject /*Thiz*/, jbyteArray FrameBytes, jint Width, jint Height)
	{
		if (!Env || !FrameBytes || Width <= 0 || Height <= 0) return;

		const jsize ByteCount = Env->GetArrayLength(FrameBytes);
		if (ByteCount <= 0) return;

		TArray<uint8> LocalCopy;
		LocalCopy.SetNumUninitialized(ByteCount);
		Env->GetByteArrayRegion(FrameBytes, 0, ByteCount, reinterpret_cast<jbyte*>(LocalCopy.GetData()));

		FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
		GAbxrPicoQrBridgeState.LatestBgraFrame = MoveTemp(LocalCopy);
		GAbxrPicoQrBridgeState.LatestFrameWidth = Width;
		GAbxrPicoQrBridgeState.LatestFrameHeight = Height;
		GAbxrPicoQrBridgeState.bHasNewFrame = true;
	}

	void JNICALL NativeOnAbxrQrPayload(JNIEnv* Env, jobject /*Thiz*/, jstring Payload)
	{
		FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
		GAbxrPicoQrBridgeState.LatestPayload = JStringToFString(Env, Payload);
		GAbxrPicoQrBridgeState.bHasPayload = !GAbxrPicoQrBridgeState.LatestPayload.IsEmpty();
	}

	void JNICALL NativeOnAbxrQrStatus(JNIEnv* Env, jobject /*Thiz*/, jint StatusCode, jstring Message)
	{
		FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
		GAbxrPicoQrBridgeState.LatestStatusCode = static_cast<int32>(StatusCode);
		GAbxrPicoQrBridgeState.LatestStatusMessage = JStringToFString(Env, Message);
		GAbxrPicoQrBridgeState.bHasStatus = true;
	}
#endif
}

bool UAbxrPicoQrScanner::RegisterJavaBridge()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("PICO QR: failed to get JNI environment"));
		return false;
	}

	FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
	if (GAbxrPicoQrBridgeState.bNativesRegistered)
	{
		return true;
	}

	jobject GameActivity = FAndroidApplication::GetGameActivityThis();
	if (!GameActivity)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("PICO QR: failed to get GameActivity instance"));
		return false;
	}

	jclass GameActivityClass = Env->GetObjectClass(GameActivity);
	if (!GameActivityClass)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("PICO QR: failed to get GameActivity class"));
		return false;
	}

	JNINativeMethod Methods[] = {
		{"nativeOnAbxrQrFrame", "([BII)V", reinterpret_cast<void*>(&NativeOnAbxrQrFrame)},
		{"nativeOnAbxrQrPayload", "(Ljava/lang/String;)V", reinterpret_cast<void*>(&NativeOnAbxrQrPayload)},
		{"nativeOnAbxrQrStatus", "(ILjava/lang/String;)V", reinterpret_cast<void*>(&NativeOnAbxrQrStatus)},
	};

	const jint Result = Env->RegisterNatives(GameActivityClass, Methods, UE_ARRAY_COUNT(Methods));
	Env->DeleteLocalRef(GameActivityClass);

	if (Result < 0)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("PICO QR: failed to register native methods on GameActivity"));
		return false;
	}

	GAbxrPicoQrBridgeState.bNativesRegistered = true;
	UE_LOG(LogAbxrLib, Log, TEXT("PICO QR: registered GameActivity native methods"));
	return true;
#else
	return false;
#endif
}

bool UAbxrPicoQrScanner::QueryAvailability() const
{
	return AbxrQrScanCommon::IsLikelyPicoDevice();
}

bool UAbxrPicoQrScanner::StartPlatformScan()
{
	TimeSinceStartSeconds = 0.0f;
	bHasEnteredScanningState = false;
	bPendingFailure = false;
	PendingFailureReason.Reset();
	PendingPayload.Reset();

#if PLATFORM_ANDROID
	if (!RegisterJavaBridge())
	{
		SetStatus(TEXT("QR scanner bridge failed to initialize"));
		return false;
	}

	{
		FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
		GAbxrPicoQrBridgeState.ActiveScanner = this;
		GAbxrPicoQrBridgeState.LatestBgraFrame.Reset();
		GAbxrPicoQrBridgeState.LatestFrameWidth = 0;
		GAbxrPicoQrBridgeState.LatestFrameHeight = 0;
		GAbxrPicoQrBridgeState.bHasNewFrame = false;
		GAbxrPicoQrBridgeState.LatestPayload.Reset();
		GAbxrPicoQrBridgeState.bHasPayload = false;
		GAbxrPicoQrBridgeState.LatestStatusCode = INDEX_NONE;
		GAbxrPicoQrBridgeState.LatestStatusMessage.Reset();
		GAbxrPicoQrBridgeState.bHasStatus = false;
	}

	if (!CallJavaVoidMethod("AndroidThunkJava_AbxrQrStart"))
	{
		SetStatus(TEXT("Failed to start Android QR scanner"));
		return false;
	}
#endif

	if (!Super::StartPlatformScan()) return false;

	SetStatus(TEXT("Starting camera..."));
	UE_LOG(LogAbxrLib, Log, TEXT("PICO QR: start requested"));
	return true;
}

void UAbxrPicoQrScanner::StopPlatformScan()
{
#if PLATFORM_ANDROID
	CallJavaVoidMethod("AndroidThunkJava_AbxrQrStop");

	FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
	if (GAbxrPicoQrBridgeState.ActiveScanner.Get() == this)
	{
		GAbxrPicoQrBridgeState.ActiveScanner = nullptr;
	}
	GAbxrPicoQrBridgeState.LatestBgraFrame.Reset();
	GAbxrPicoQrBridgeState.bHasNewFrame = false;
	GAbxrPicoQrBridgeState.LatestPayload.Reset();
	GAbxrPicoQrBridgeState.bHasPayload = false;
	GAbxrPicoQrBridgeState.LatestStatusCode = INDEX_NONE;
	GAbxrPicoQrBridgeState.LatestStatusMessage.Reset();
	GAbxrPicoQrBridgeState.bHasStatus = false;
#endif

	PreviewTexture = nullptr;
	PendingPayload.Reset();
	PendingFailureReason.Reset();
	bPendingFailure = false;
	bHasEnteredScanningState = false;
	TimeSinceStartSeconds = 0.0f;
}

void UAbxrPicoQrScanner::TickPlatformScan(float DeltaTime)
{
	TimeSinceStartSeconds += DeltaTime;
	ConsumeBridgeUpdates();

	if (!bHasEnteredScanningState && TimeSinceStartSeconds > StartupTimeoutSeconds)
	{
		FailAndCancel(TEXT("PICO QR camera did not start before timeout"));
		return;
	}

	if (bPendingFailure)
	{
		FailAndCancel(PendingFailureReason);
	}
}

bool UAbxrPicoQrScanner::ConsumeLatestPayload(FString& OutRawPayload)
{
	if (PendingPayload.IsEmpty()) return false;
	OutRawPayload = MoveTemp(PendingPayload);
	PendingPayload.Reset();
	return true;
}

UTexture2D* UAbxrPicoQrScanner::GetPreviewTexture() const
{
	return PreviewTexture;
}

bool UAbxrPicoQrScanner::CallJavaVoidMethod(const ANSICHAR* MethodName) const
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobject GameActivity = FAndroidApplication::GetGameActivityThis();
	if (!Env || !GameActivity) return false;

	jclass GameActivityClass = Env->GetObjectClass(GameActivity);
	if (!GameActivityClass) return false;

	jmethodID Method = Env->GetMethodID(GameActivityClass, MethodName, "()V");
	if (!Method)
	{
		Env->DeleteLocalRef(GameActivityClass);
		UE_LOG(LogAbxrLib, Error, TEXT("PICO QR: Java method %s not found"), UTF8_TO_TCHAR(MethodName));
		return false;
	}

	Env->CallVoidMethod(GameActivity, Method);
	Env->DeleteLocalRef(GameActivityClass);
	return !Env->ExceptionCheck();
#else
	return false;
#endif
}

void UAbxrPicoQrScanner::ConsumeBridgeUpdates()
{
#if PLATFORM_ANDROID
	TArray<uint8> LocalFrame;
	int32 LocalWidth = 0;
	int32 LocalHeight = 0;
	FString LocalPayload;
	int32 LocalStatusCode = INDEX_NONE;
	FString LocalStatusMessage;
	bool bHasLocalStatus = false;

	{
		FScopeLock Lock(&GAbxrPicoQrBridgeState.Mutex);
		if (GAbxrPicoQrBridgeState.bHasNewFrame)
		{
			LocalFrame = MoveTemp(GAbxrPicoQrBridgeState.LatestBgraFrame);
			LocalWidth = GAbxrPicoQrBridgeState.LatestFrameWidth;
			LocalHeight = GAbxrPicoQrBridgeState.LatestFrameHeight;
			GAbxrPicoQrBridgeState.bHasNewFrame = false;
		}

		if (GAbxrPicoQrBridgeState.bHasPayload)
		{
			LocalPayload = MoveTemp(GAbxrPicoQrBridgeState.LatestPayload);
			GAbxrPicoQrBridgeState.bHasPayload = false;
		}

		if (GAbxrPicoQrBridgeState.bHasStatus)
		{
			LocalStatusCode = GAbxrPicoQrBridgeState.LatestStatusCode;
			LocalStatusMessage = MoveTemp(GAbxrPicoQrBridgeState.LatestStatusMessage);
			GAbxrPicoQrBridgeState.bHasStatus = false;
			bHasLocalStatus = true;
		}
	}

	if (bHasLocalStatus)
	{
		switch (static_cast<EBridgeStatus>(LocalStatusCode))
		{
		case EBridgeStatus::Starting:
			SetStatus(LocalStatusMessage.IsEmpty() ? TEXT("Starting camera...") : LocalStatusMessage);
			break;
		case EBridgeStatus::Scanning:
			bHasEnteredScanningState = true;
			SetStatus(LocalStatusMessage.IsEmpty() ? TEXT("Look at QR Code") : LocalStatusMessage);
			break;
		case EBridgeStatus::PermissionDenied:
			bPendingFailure = true;
			PendingFailureReason = LocalStatusMessage.IsEmpty() ? TEXT("Camera permission denied") : LocalStatusMessage;
			break;
		case EBridgeStatus::Error:
		default:
			bPendingFailure = true;
			PendingFailureReason = LocalStatusMessage.IsEmpty() ? TEXT("PICO QR scanner failed") : LocalStatusMessage;
			break;
		}
	}

	if (LocalWidth > 0 && LocalHeight > 0 && LocalFrame.Num() == LocalWidth * LocalHeight * 4)
	{
		UpdatePreviewTextureFromBGRA(LocalFrame, LocalWidth, LocalHeight);
	}

	if (!LocalPayload.IsEmpty())
	{
		PendingPayload = MoveTemp(LocalPayload);
	}
#endif
}

void UAbxrPicoQrScanner::UpdatePreviewTextureFromBGRA(const TArray<uint8>& InBgraBytes, int32 Width, int32 Height)
{
	if (Width <= 0 || Height <= 0 || InBgraBytes.Num() != Width * Height * 4) return;

	if (!PreviewTexture || PreviewTexture->GetSizeX() != Width || PreviewTexture->GetSizeY() != Height)
	{
		PreviewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (!PreviewTexture) return;
		PreviewTexture->SRGB = false;
		PreviewTexture->NeverStream = true;
		PreviewTexture->Filter = TF_Bilinear;
		PreviewTexture->UpdateResource();
	}

	FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
	uint8* DataCopy = static_cast<uint8*>(FMemory::Malloc(InBgraBytes.Num()));
	FMemory::Memcpy(DataCopy, InBgraBytes.GetData(), InBgraBytes.Num());

	PreviewTexture->UpdateTextureRegions(
		0,
		1,
		Region,
		static_cast<uint32>(Width * 4),
		4,
		DataCopy,
		[](uint8* SrcData, const FUpdateTextureRegion2D* SrcRegions)
		{
			FMemory::Free(SrcData);
			delete SrcRegions;
		});
}

void UAbxrPicoQrScanner::FailAndCancel(const FString& Reason)
{
	UE_LOG(LogAbxrLib, Warning, TEXT("PICO QR: %s"), *Reason);
	SetStatus(Reason);
	bPendingFailure = false;
	PendingFailureReason.Reset();
	FinishScan(TEXT(""), true);
}
