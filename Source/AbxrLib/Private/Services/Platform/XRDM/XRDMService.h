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
    static UXRDMService* GetInstance();
    static void DestroyInstance();
    
    void Initialize();
    void Shutdown();
    
    // Connection waiting - returns a future that completes when connection attempt finishes
    TSharedPtr<TPromise<bool>> WaitForConnection();
    
    // Synchronous connection check (returns immediately)
    bool IsConnectionAttemptComplete() const;
    static bool IsConnected();
    
    static FString GetDeviceId();
    static FString GetDeviceSerial();
    static FString GetDeviceTitle();
    static TArray<FString> GetDeviceTags();
    static FString GetOrgId();
    static FString GetOrgTitle();
    static FString GetOrgSlug();
    static FString GetMacAddressFixed();
    static FString GetMacAddressRandom();
    static bool GetIsAuthenticated();
    static FString GetAccessToken();
    static FString GetRefreshToken();
    static FDateTime GetExpiresDateUtc();
    static bool GetIsInitialized();
    static FString GetFingerprint();

protected:
    UXRDMService();  // Prevent direct construction
    
    // Override BeginDestroy to handle cleanup
    virtual void BeginDestroy() override;

private:
    static UXRDMService* Instance;
    bool bIsInitialized = false;
    bool bConnectionAttemptComplete = false;
    TArray<TSharedPtr<TPromise<bool>>> PendingConnectionPromises;
    FTSTicker::FDelegateHandle ConnectionTimeoutHandle;
    
    // Private helper for completing connection attempts
    void CompleteConnectionAttempt(bool bSuccess);

    bool OnConnectionTimeout(float DeltaTime);

    bool bIsConnected = false;
    
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
    
    // JNI callback handling
    static void RegisterNativeMethods();
    static void JNICALL NativeOnConnected(JNIEnv* Env, jclass Clazz, jobject Service);
    static void JNICALL NativeOnDisconnected(JNIEnv* Env, jclass Clazz, jboolean WasClean);
#endif
};