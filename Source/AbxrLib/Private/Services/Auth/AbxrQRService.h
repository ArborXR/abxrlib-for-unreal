#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQRCodeScanned, const FString& DecodedText);

class FAbxrQRService
{
public:
	FAbxrQRService();
	~FAbxrQRService();

	// Not copyable — owns JNI global refs
	FAbxrQRService(const FAbxrQRService&) = delete;
	FAbxrQRService& operator=(const FAbxrQRService&) = delete;

	void Initialize();
	void Shutdown();
	
	static bool IsSupported();
	void StartScan();
	void StopScan();

	bool IsScanning() const { return bIsScanning; }

	FOnQRCodeScanned OnQRCodeScanned;

private:
	bool bIsInitialized = false;
	bool bIsScanning = false;

#if PLATFORM_ANDROID
	jclass QRScannerClass = nullptr;
	jobject QRScannerObject = nullptr;

	void InitJava();
	void CleanupJNI();
	
	static void RegisterNativeMethods();
	
	static void JNICALL NativeOnScanned(JNIEnv* Env, jclass Class, jstring Result);
	static void JNICALL NativeOnFailed(JNIEnv* Env, jclass Class, jstring Reason);
	static void JNICALL NativeOnCancelled(JNIEnv* Env, jclass Class);
	
	static FAbxrQRService* ActiveInstance;
#endif
};
