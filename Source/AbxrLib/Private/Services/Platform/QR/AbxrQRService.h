#pragma once
#include "CoreMinimal.h"
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbxrQRCodeScanned, const FString&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbxrQRScanFailed, const FString&);
DECLARE_MULTICAST_DELEGATE(FOnAbxrQRScanCancelled);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAbxrQRPreviewFrame, const TArray<uint8>&, int32, int32);

class ABXRLIB_API FAbxrQRService
{
public:
	FAbxrQRService();
	~FAbxrQRService();

	void Initialize();
	void Shutdown();

	bool StartScan();
	void StopScan();
	bool IsScanning() const { return bIsScanning; }

	static bool IsSupported();

	FOnAbxrQRCodeScanned OnQRCodeScanned;
	FOnAbxrQRScanFailed OnQRScanFailed;
	FOnAbxrQRScanCancelled OnQRScanCancelled;
	FOnAbxrQRPreviewFrame OnQRPreviewFrame;

private:
	bool bIsInitialized = false;
	bool bIsScanning = false;

#if PLATFORM_ANDROID
	jclass ScannerClass = nullptr;
	jobject ScannerInstance = nullptr;

	static FAbxrQRService* ActiveInstance;
	static bool bNativeMethodsRegistered;

	bool RegisterNativeMethods();
	bool EnsureScannerInstance();
	void CleanupJNI();

	static void JNICALL NativeOnScanned(JNIEnv* Env, jclass Clazz, jstring DecodedText);
	static void JNICALL NativeOnFailed(JNIEnv* Env, jclass Clazz, jstring Reason);
	static void JNICALL NativeOnCancelled(JNIEnv* Env, jclass Clazz);
	static void JNICALL NativeOnPreviewFrame(JNIEnv* Env, jclass Clazz, jbyteArray RgbaBytes, jint Width, jint Height);
#endif
};
