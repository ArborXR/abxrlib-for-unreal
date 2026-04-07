#pragma once
#include "CoreMinimal.h"
#include "Async/Future.h"
#include "Containers/Ticker.h"
#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif
#include "XRDMService.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnXRDMConnected);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXRDMConnectionFailed, const FString&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXRDMDisconnected, bool);

UCLASS()
class ABXRLIB_API UXRDMService : public UObject
{
    GENERATED_BODY()

public:
    void Initialize();
    void Shutdown();

    TSharedPtr<TPromise<bool>> WaitForConnection();

    bool IsConnectionAttemptComplete() const;
    bool IsConnected() const;

    FString GetDeviceId() const;
    FString GetDeviceSerial() const;
    FString GetDeviceTitle() const;
    TArray<FString> GetDeviceTags() const;
    FString GetOrgId() const;
    FString GetOrgTitle() const;
    FString GetOrgSlug() const;
    FString GetMacAddressFixed() const;
    FString GetMacAddressRandom() const;
    bool GetIsAuthenticated() const;
    FString GetAccessToken() const;
    FString GetRefreshToken() const;
    FDateTime GetExpiresDateUtc() const;
    bool GetIsInitialized() const;
    FString GetFingerprint() const;

    /** Logs connection flags and JNI state for debugging MDM/XRDM integration (LogAbxrLib). */
    void LogXrdmConnectionState(const TCHAR* Context) const;

protected:
    virtual void BeginDestroy() override;

private:
    static constexpr int32 MaxConnectionAttempts = 3;
    static constexpr float ConnectionTimeoutSeconds = 2.0f;
    static constexpr float ConnectionRetryDelaySeconds = 0.5f;

    bool bIsInitialized = false;
    bool bConnectionAttemptComplete = false;
    bool bConnectionInProgress = false;
    bool bConnectCallIssued = false;
    bool bIsConnected = false;
    int32 ConnectionAttemptCount = 0;

    TArray<TSharedPtr<TPromise<bool>>> PendingConnectionPromises;
    FTSTicker::FDelegateHandle ConnectionTimeoutHandle;
    FTSTicker::FDelegateHandle ConnectionRetryHandle;

    void BeginConnectionAttempt();
    void CompleteConnectionAttempt(bool bSuccess);
    void HandleRetryableFailure(const TCHAR* Reason);
    bool OnConnectionTimeout(float DeltaTime);
    bool OnConnectionRetry(float DeltaTime);

#if PLATFORM_ANDROID
    jclass SdkClass = nullptr;
    jobject SdkInstance = nullptr;
    jobject ServiceWrapper = nullptr;
    jobject ConnectionCallback = nullptr;

    void InitializeSDK();
    void ConnectToService();
    void CleanupJNI();

    FString CallJNIResultString(const char* MethodName) const;
    bool CallJNIBoolMethod(const char* MethodName) const;
    FDateTime CallJNIDateTimeMethod(const char* MethodName) const;
    TArray<FString> CallJNIStringArrayMethod(const char* MethodName) const;
    jobject CallJNIResultMethod(const char* MethodName) const;

    static bool RegisterNativeMethods();
    static void JNICALL NativeOnConnected(JNIEnv* Env, jclass Clazz, jobject Service);
    static void JNICALL NativeOnDisconnected(JNIEnv* Env, jclass Clazz, jboolean WasClean);

    static TWeakObjectPtr<UXRDMService> ActiveInstance;
    static bool bNativeMethodsRegistered;
#endif
};
