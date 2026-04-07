#include "XRDMService.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Async/AsyncWork.h"
#include "Async/Async.h"
#include "Types/AbxrLog.h"
#include "UObject/Package.h"

#if PLATFORM_ANDROID
TWeakObjectPtr<UXRDMService> UXRDMService::ActiveInstance;
bool UXRDMService::bNativeMethodsRegistered = false;
#endif

void UXRDMService::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

void UXRDMService::Initialize()
{
    if (bIsInitialized) return;

    bIsInitialized = true;
    bConnectionAttemptComplete = false;
    bConnectionInProgress = false;
    bConnectCallIssued = false;
    bIsConnected = false;
    ConnectionAttemptCount = 0;

#if PLATFORM_ANDROID
    ActiveInstance = this;

    if (!bNativeMethodsRegistered)
    {
        bNativeMethodsRegistered = RegisterNativeMethods();
        if (!bNativeMethodsRegistered)
        {
            UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Initialize: native registration not ready yet; retries will continue during connection attempts"));
        }
    }

    UE_LOG(LogAbxrLib, Log, TEXT("XRDM Initialize: registering JNI bridge and starting SDK connect"));
    BeginConnectionAttempt();
#endif
}

void UXRDMService::Shutdown()
{
    if (!bIsInitialized) return;

    bIsInitialized = false;
    bConnectionAttemptComplete = false;
    bConnectionInProgress = false;
    bConnectCallIssued = false;
    bIsConnected = false;

    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }

    if (ConnectionRetryHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionRetryHandle);
        ConnectionRetryHandle.Reset();
    }

    for (auto& Promise : PendingConnectionPromises)
    {
        if (Promise.IsValid())
        {
            Promise->SetValue(false);
        }
    }
    PendingConnectionPromises.Empty();

#if PLATFORM_ANDROID
    if (ActiveInstance.Get() == this)
    {
        ActiveInstance = nullptr;
    }

    CleanupJNI();
#endif
}

TSharedPtr<TPromise<bool>> UXRDMService::WaitForConnection()
{
    auto Promise = MakeShared<TPromise<bool>>();

    if (!bIsInitialized)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM WaitForConnection: not initialized, returning false"));
        Promise->SetValue(false);
        return Promise;
    }

    if (bConnectionAttemptComplete)
    {
        UE_LOG(LogAbxrLib, Log, TEXT("XRDM WaitForConnection: already complete, connected=%d"), bIsConnected ? 1 : 0);
        Promise->SetValue(bIsConnected);
        return Promise;
    }

    PendingConnectionPromises.Add(Promise);

    if (!bConnectionInProgress)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM WaitForConnection: no connection in progress; starting/restarting attempt"));
        BeginConnectionAttempt();
    }

    return Promise;
}

void UXRDMService::BeginConnectionAttempt()
{
#if PLATFORM_ANDROID
    if (!bIsInitialized || bConnectionAttemptComplete || bConnectionInProgress) return;

    if (ConnectionRetryHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionRetryHandle);
        ConnectionRetryHandle.Reset();
    }

    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }

    ++ConnectionAttemptCount;
    bConnectionInProgress = true;
    bConnectCallIssued = false;

    UE_LOG(LogAbxrLib, Log, TEXT("XRDM BeginConnectionAttempt #%d"), ConnectionAttemptCount);

    if (!bNativeMethodsRegistered)
    {
        bNativeMethodsRegistered = RegisterNativeMethods();
        if (!bNativeMethodsRegistered)
        {
            HandleRetryableFailure(TEXT("RegisterNativeMethods failed"));
            return;
        }
    }

    InitializeSDK();
#else
    CompleteConnectionAttempt(false);
#endif
}

bool UXRDMService::OnConnectionTimeout(float DeltaTime)
{
    if (!bConnectionAttemptComplete)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Connection attempt #%d timed out after %.1fs (no nativeOnConnected, or JNI/SDK failure)"), ConnectionAttemptCount, ConnectionTimeoutSeconds);
        HandleRetryableFailure(TEXT("Connection timeout"));
    }

    return false;
}

bool UXRDMService::OnConnectionRetry(float DeltaTime)
{
    ConnectionRetryHandle.Reset();

    if (!bIsInitialized || bConnectionAttemptComplete || bConnectionInProgress) return false;

    BeginConnectionAttempt();
    return false;
}

void UXRDMService::HandleRetryableFailure(const TCHAR* Reason)
{
    bConnectionInProgress = false;
    bConnectCallIssued = false;
    bIsConnected = false;

#if PLATFORM_ANDROID
    CleanupJNI();
#endif

    if (!bIsInitialized || bConnectionAttemptComplete)
    {
        return;
    }

    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }

    if (ConnectionAttemptCount < MaxConnectionAttempts)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM attempt #%d failed: %s. Retrying in %.2fs"), ConnectionAttemptCount, Reason, ConnectionRetryDelaySeconds);
        if (!ConnectionRetryHandle.IsValid())
        {
            ConnectionRetryHandle = FTSTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateUObject(this, &UXRDMService::OnConnectionRetry),
                ConnectionRetryDelaySeconds
            );
        }
        return;
    }

    UE_LOG(LogAbxrLib, Error, TEXT("XRDM attempt #%d failed: %s. No retries left"), ConnectionAttemptCount, Reason);
    CompleteConnectionAttempt(false);
}

bool UXRDMService::IsConnectionAttemptComplete() const
{
    return bConnectionAttemptComplete;
}

void UXRDMService::CompleteConnectionAttempt(bool bSuccess)
{
    if (bConnectionAttemptComplete && bIsConnected == bSuccess) return;

    bConnectionAttemptComplete = true;
    bConnectionInProgress = false;
    bConnectCallIssued = false;
    bIsConnected = bSuccess;
    UE_LOG(LogAbxrLib, Log, TEXT("XRDM CompleteConnectionAttempt: success=%d"), bSuccess ? 1 : 0);

    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }

    if (ConnectionRetryHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionRetryHandle);
        ConnectionRetryHandle.Reset();
    }

    for (auto& Promise : PendingConnectionPromises)
    {
        if (Promise.IsValid())
        {
            Promise->SetValue(bSuccess);
        }
    }
    PendingConnectionPromises.Empty();
}

bool UXRDMService::IsConnected() const
{
#if PLATFORM_ANDROID
    return bIsConnected && ServiceWrapper != nullptr;
#else
    return false;
#endif
}

FString UXRDMService::GetDeviceId() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getDeviceId");
#else
    return FString();
#endif
}

FString UXRDMService::GetDeviceSerial() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getDeviceSerial");
#else
    return FString();
#endif
}

FString UXRDMService::GetDeviceTitle() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getDeviceTitle");
#else
    return FString();
#endif
}

TArray<FString> UXRDMService::GetDeviceTags() const
{
    if (!IsConnected()) return TArray<FString>();
    
#if PLATFORM_ANDROID
    return CallJNIStringArrayMethod("getDeviceTags");
#else
    return TArray<FString>();
#endif
}

FString UXRDMService::GetOrgId() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getOrgId");
#else
    return FString();
#endif
}

FString UXRDMService::GetOrgTitle() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getOrgTitle");
#else
    return FString();
#endif
}

FString UXRDMService::GetOrgSlug() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getOrgSlug");
#else
    return FString();
#endif
}

FString UXRDMService::GetMacAddressFixed() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getMacAddressFixed");
#else
    return FString();
#endif
}

FString UXRDMService::GetMacAddressRandom() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getMacAddressRandom");
#else
    return FString();
#endif
}

bool UXRDMService::GetIsAuthenticated() const
{
    if (!IsConnected()) return false;
    
#if PLATFORM_ANDROID
    return CallJNIBoolMethod("getIsAuthenticated");
#else
    return false;
#endif
}

FString UXRDMService::GetAccessToken() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getAccessToken");
#else
    return FString();
#endif
}

FString UXRDMService::GetRefreshToken() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getRefreshToken");
#else
    return FString();
#endif
}

FDateTime UXRDMService::GetExpiresDateUtc() const
{
    if (!IsConnected()) return FDateTime::MinValue();
    
#if PLATFORM_ANDROID
    return CallJNIDateTimeMethod("getExpiresDateUtc");
#else
    return FDateTime();
#endif
}

bool UXRDMService::GetIsInitialized() const
{
    if (!IsConnected()) return false;
    
#if PLATFORM_ANDROID
    return CallJNIBoolMethod("getIsInitialized");
#else
    return false;
#endif
}

FString UXRDMService::GetFingerprint() const
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    return CallJNIResultString("getFingerprint");
#else
    return FString();
#endif
}

#if PLATFORM_ANDROID

bool UXRDMService::RegisterNativeMethods()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to get JNI environment for native method registration"));
        return false;
    }

    jclass CallbackClass = FAndroidApplication::FindJavaClass("com.xrdm.xrdmbridge.NativeConnectionCallback");
    if (!CallbackClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find NativeConnectionCallback class for native method registration"));
        return false;
    }

    JNINativeMethod NativeMethods[] = {
        {"nativeOnConnected", "(Ljava/lang/Object;)V", (void*)&UXRDMService::NativeOnConnected},
        {"nativeOnDisconnected", "(Z)V", (void*)&UXRDMService::NativeOnDisconnected}
    };

    const int Result = Env->RegisterNatives(CallbackClass, NativeMethods, 2);
    if (Result < 0)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to register native methods"));
        return false;
    }

    UE_LOG(LogAbxrLib, Log, TEXT("XRDM Successfully registered XRDM native methods (onConnected & onDisconnected)"));
    return true;
}

void JNICALL UXRDMService::NativeOnConnected(JNIEnv* Env, jclass Clazz, jobject Service)
{
    UXRDMService* LocalInstance = ActiveInstance.Get();
    if (!LocalInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM nativeOnConnected: ActiveInstance is null (XRDMService not initialized)"));
        return;
    }

    if (!Service)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM nativeOnConnected: service object is null; connection failed"));
        AsyncTask(ENamedThreads::GameThread, [WeakInstance = TWeakObjectPtr<UXRDMService>(LocalInstance)]()
        {
            if (UXRDMService* StrongInstance = WeakInstance.Get())
            {
                StrongInstance->HandleRetryableFailure(TEXT("nativeOnConnected received null service"));
            }
        });
        return;
    }

    if (LocalInstance->bConnectionAttemptComplete && LocalInstance->bIsConnected)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM nativeOnConnected received after successful connection; ignoring duplicate callback"));
        return;
    }

    if (LocalInstance->ServiceWrapper)
    {
        Env->DeleteGlobalRef(LocalInstance->ServiceWrapper);
        LocalInstance->ServiceWrapper = nullptr;
    }

    LocalInstance->ServiceWrapper = Env->NewGlobalRef(Service);
    LocalInstance->bIsConnected = LocalInstance->ServiceWrapper != nullptr;

    AsyncTask(ENamedThreads::GameThread, [WeakInstance = TWeakObjectPtr<UXRDMService>(LocalInstance)]()
    {
        if (UXRDMService* StrongInstance = WeakInstance.Get())
        {
            if (StrongInstance->bIsInitialized)
            {
                if (StrongInstance->ServiceWrapper)
                {
                    StrongInstance->CompleteConnectionAttempt(true);
                    UE_LOG(LogAbxrLib, Log, TEXT("XRDM SDK connected via native callback on attempt #%d"), StrongInstance->ConnectionAttemptCount);
                }
                else
                {
                    StrongInstance->HandleRetryableFailure(TEXT("nativeOnConnected could not create service global ref"));
                }
            }
            else
            {
                UE_LOG(LogAbxrLib, Warning, TEXT("XRDM nativeOnConnected after XRDMService shutdown; connection attempt may time out"));
            }
        }
    });
}

void JNICALL UXRDMService::NativeOnDisconnected(JNIEnv* Env, jclass Clazz, jboolean WasClean)
{
    UXRDMService* LocalInstance = ActiveInstance.Get();
    if (LocalInstance)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM nativeOnDisconnected wasClean=%d"), WasClean == JNI_TRUE ? 1 : 0);
        if (LocalInstance->ServiceWrapper)
        {
            Env->DeleteGlobalRef(LocalInstance->ServiceWrapper);
            LocalInstance->ServiceWrapper = nullptr;
        }
        LocalInstance->bIsConnected = false;
        LocalInstance->bConnectionInProgress = false;
        LocalInstance->bConnectCallIssued = false;
        LocalInstance->bConnectionAttemptComplete = true;
    }
}

void UXRDMService::InitializeSDK()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to get JNI environment"));
        HandleRetryableFailure(TEXT("Failed to get JNI environment"));
        return;
    }

    SdkClass = FAndroidApplication::FindJavaClass("app.xrdm.sdk.external.Sdk");
    if (!SdkClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find any SDK class"));
        HandleRetryableFailure(TEXT("Failed to find SDK class"));
        return;
    }

    jmethodID Constructor = Env->GetMethodID(SdkClass, "<init>", "()V");
    if (!Constructor)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find SDK constructor"));
        HandleRetryableFailure(TEXT("Failed to find SDK constructor"));
        return;
    }

    jobject LocalSdkInstance = Env->NewObject(SdkClass, Constructor);
    if (!LocalSdkInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to create SDK instance"));
        HandleRetryableFailure(TEXT("Failed to create SDK instance"));
        return;
    }

    SdkInstance = Env->NewGlobalRef(LocalSdkInstance);
    Env->DeleteLocalRef(LocalSdkInstance);
    if (!SdkInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to promote SDK instance to global ref"));
        HandleRetryableFailure(TEXT("Failed to create SDK global ref"));
        return;
    }

    UE_LOG(LogAbxrLib, Log, TEXT("XRDM SDK instance created successfully"));
    ConnectToService();
}

void UXRDMService::ConnectToService()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env || !SdkInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM JNI environment or SDK instance not available"));
        HandleRetryableFailure(TEXT("JNI environment or SDK instance not available"));
        return;
    }

    jobject CurrentActivity = FAndroidApplication::GetGameActivityThis();
    if (!CurrentActivity)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to get current activity"));
        HandleRetryableFailure(TEXT("Failed to get current activity"));
        return;
    }

    jclass CallbackClass = FAndroidApplication::FindJavaClass("com.xrdm.xrdmbridge.NativeConnectionCallback");
    if (!CallbackClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find NativeConnectionCallback class"));
        HandleRetryableFailure(TEXT("Failed to find NativeConnectionCallback class"));
        return;
    }

    jmethodID CallbackConstructor = Env->GetMethodID(CallbackClass, "<init>", "()V");
    if (!CallbackConstructor)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find NativeConnectionCallback constructor"));
        HandleRetryableFailure(TEXT("Failed to find NativeConnectionCallback constructor"));
        return;
    }

    jobject CallbackInstance = Env->NewObject(CallbackClass, CallbackConstructor);
    if (!CallbackInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to create NativeConnectionCallback instance"));
        HandleRetryableFailure(TEXT("Failed to create NativeConnectionCallback instance"));
        return;
    }

    if (ConnectionCallback)
    {
        Env->DeleteGlobalRef(ConnectionCallback);
        ConnectionCallback = nullptr;
    }

    ConnectionCallback = Env->NewGlobalRef(CallbackInstance);
    Env->DeleteLocalRef(CallbackInstance);
    if (!ConnectionCallback)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to promote NativeConnectionCallback to global ref"));
        HandleRetryableFailure(TEXT("Failed to create callback global ref"));
        return;
    }

    jmethodID ConnectMethod = Env->GetMethodID(SdkClass, "connect", "(Landroid/content/Context;Lapp/xrdm/sdk/external/IConnectionCallback;)V");
    if (ConnectMethod)
    {
        UE_LOG(LogAbxrLib, Log, TEXT("XRDM Using Sdk.connect(Context, IConnectionCallback) on attempt #%d"), ConnectionAttemptCount);
        bConnectCallIssued = true;
        Env->CallVoidMethod(SdkInstance, ConnectMethod, CurrentActivity, ConnectionCallback);
        if (Env->ExceptionCheck())
        {
            UE_LOG(LogAbxrLib, Error, TEXT("XRDM Exception from Sdk.connect(Context, ...)"));
            Env->ExceptionDescribe();
            Env->ExceptionClear();
            HandleRetryableFailure(TEXT("Exception from Sdk.connect(Context, ...)"));
            return;
        }

        UE_LOG(LogAbxrLib, Log, TEXT("XRDM Sdk.connect() returned; waiting for NativeConnectionCallback.onConnected (or timeout)"));
        if (!ConnectionTimeoutHandle.IsValid())
        {
            ConnectionTimeoutHandle = FTSTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateUObject(this, &UXRDMService::OnConnectionTimeout),
                ConnectionTimeoutSeconds
            );
        }
        return;
    }

    ConnectMethod = Env->GetMethodID(SdkClass, "connect", "(Landroid/app/Activity;Lapp/xrdm/sdk/external/IConnectionCallback;)V");
    if (ConnectMethod)
    {
        UE_LOG(LogAbxrLib, Log, TEXT("XRDM Using Sdk.connect(Activity, IConnectionCallback) on attempt #%d"), ConnectionAttemptCount);
        bConnectCallIssued = true;
        Env->CallVoidMethod(SdkInstance, ConnectMethod, CurrentActivity, ConnectionCallback);
        if (Env->ExceptionCheck())
        {
            UE_LOG(LogAbxrLib, Error, TEXT("XRDM Exception from Sdk.connect(Activity, ...)"));
            Env->ExceptionDescribe();
            Env->ExceptionClear();
            HandleRetryableFailure(TEXT("Exception from Sdk.connect(Activity, ...)"));
            return;
        }

        UE_LOG(LogAbxrLib, Log, TEXT("XRDM Sdk.connect() returned; waiting for NativeConnectionCallback.onConnected (or timeout)"));
        if (!ConnectionTimeoutHandle.IsValid())
        {
            ConnectionTimeoutHandle = FTSTicker::GetCoreTicker().AddTicker(
                FTickerDelegate::CreateUObject(this, &UXRDMService::OnConnectionTimeout),
                ConnectionTimeoutSeconds
            );
        }
        return;
    }

    Env->DeleteGlobalRef(ConnectionCallback);
    ConnectionCallback = nullptr;

    UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find connect method with IConnectionCallback"));
    HandleRetryableFailure(TEXT("Failed to find connect method with IConnectionCallback"));
}

void UXRDMService::CleanupJNI()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (Env)
    {
        if (ServiceWrapper)
        {
            Env->DeleteGlobalRef(ServiceWrapper);
            ServiceWrapper = nullptr;
        }
        
        if (SdkInstance)
        {
            Env->DeleteGlobalRef(SdkInstance);
            SdkInstance = nullptr;
        }
        
        if (ConnectionCallback)
        {
            Env->DeleteGlobalRef(ConnectionCallback);
            ConnectionCallback = nullptr;
        }
    }
    
    bIsConnected = false;
}

jobject UXRDMService::CallJNIResultMethod(const char* MethodName) const
{
    if (!bIsConnected || !ServiceWrapper)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Cannot call %s - service not connected"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }

    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM No JNI environment for %s"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }

    // Clear any pending exceptions first
    if (Env->ExceptionCheck())
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Clearing pending exception before calling %s"), UTF8_TO_TCHAR(MethodName));
        Env->ExceptionClear();
    }

    jclass ServiceWrapperClass = Env->GetObjectClass(ServiceWrapper);
    if (!ServiceWrapperClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to get service wrapper class for %s"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jmethodID Method = Env->GetMethodID(ServiceWrapperClass, MethodName, "()Lapp/xrdm/sdk/external/Result;");
    if (!Method)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Failed to find method %s with Result signature"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jobject ResultObject = Env->CallObjectMethod(ServiceWrapper, Method);
    
    if (Env->ExceptionCheck())
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Exception occurred calling %s"), UTF8_TO_TCHAR(MethodName));
        Env->ExceptionDescribe();
        Env->ExceptionClear();
        return nullptr;
    }
    
    if (!ResultObject)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Method %s returned null Result object"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jclass ResultClass = FAndroidApplication::FindJavaClass("app.xrdm.sdk.external.Result");
    if (!ResultClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Could not find Result class"));
        Env->DeleteLocalRef(ResultObject);
        return nullptr;
    }
    
    jmethodID IsOkMethod = Env->GetMethodID(ResultClass, "isOk", "()Z");
    if (!IsOkMethod)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("XRDM Could not find isOk method on Result class"));
        Env->DeleteLocalRef(ResultObject);
        return nullptr;
    }
    
    jboolean IsOk = Env->CallBooleanMethod(ResultObject, IsOkMethod);
    
    if (IsOk == JNI_TRUE)
    {
        jmethodID GetValueMethod = Env->GetMethodID(ResultClass, "getValue", "()Ljava/lang/Object;");
        if (!GetValueMethod)
        {
            UE_LOG(LogAbxrLib, Error, TEXT("XRDM Could not find getValue method on Result class"));
            Env->DeleteLocalRef(ResultObject);
            return nullptr;
        }
        
        jobject ValueObject = Env->CallObjectMethod(ResultObject, GetValueMethod);
        Env->DeleteLocalRef(ResultObject); // Clean up the Result object
        return ValueObject; // Return the actual value (caller is responsible for cleanup)
    }
    else
    {
        // Get the error message
        jmethodID GetErrorMethod = Env->GetMethodID(ResultClass, "getError", "()Ljava/lang/String;");
        if (GetErrorMethod)
        {
            jstring ErrorString = static_cast<jstring>(Env->CallObjectMethod(ResultObject, GetErrorMethod));
            if (ErrorString)
            {
                const char* ErrorChars = Env->GetStringUTFChars(ErrorString, nullptr);
                UE_LOG(LogAbxrLib, Error, TEXT("XRDM SDK Error calling %s: %s"), UTF8_TO_TCHAR(MethodName), UTF8_TO_TCHAR(ErrorChars));
                Env->ReleaseStringUTFChars(ErrorString, ErrorChars);
                Env->DeleteLocalRef(ErrorString);
            }
        }
        Env->DeleteLocalRef(ResultObject);
        return nullptr;
    }
}

FString UXRDMService::CallJNIResultString(const char* MethodName) const
{
    jobject ValueObject = CallJNIResultMethod(MethodName);
    if (!ValueObject) return FString();
    
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    jstring ValueString = static_cast<jstring>(ValueObject);
    if (ValueString)
    {
        const char* ValueChars = Env->GetStringUTFChars(ValueString, nullptr);
        FString ReturnValue(UTF8_TO_TCHAR(ValueChars));
        Env->ReleaseStringUTFChars(ValueString, ValueChars);
        Env->DeleteLocalRef(ValueObject);
        return ReturnValue;
    }
    
    UE_LOG(LogAbxrLib, Warning, TEXT("XRDM getValue() returned non-string for %s"), UTF8_TO_TCHAR(MethodName));
    Env->DeleteLocalRef(ValueObject);
    return FString();
}

bool UXRDMService::CallJNIBoolMethod(const char* MethodName) const
{
    jobject ValueObject = CallJNIResultMethod(MethodName);
    if (!ValueObject) return false;
    
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    bool ReturnValue = false;
    
    jstring ValueString = static_cast<jstring>(ValueObject);
    if (ValueString)
    {
        const char* ValueChars = Env->GetStringUTFChars(ValueString, nullptr);
        FString StringValue(UTF8_TO_TCHAR(ValueChars));
        ReturnValue = StringValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) || StringValue.Equals(TEXT("1"));
        Env->ReleaseStringUTFChars(ValueString, ValueChars);
    }
    else
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM ValueObject is not a String for method %s"), UTF8_TO_TCHAR(MethodName));
    }
    
    Env->DeleteLocalRef(ValueObject);
    return ReturnValue;
}

FDateTime UXRDMService::CallJNIDateTimeMethod(const char* MethodName) const
{
    jobject ValueObject = CallJNIResultMethod(MethodName);
    if (!ValueObject) return FDateTime();
    
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    FDateTime ReturnValue;
    
    // Convert the value object to string (should be ISO8601 datetime string)
    jstring ValueString = static_cast<jstring>(ValueObject);
    if (ValueString)
    {
        const char* ValueChars = Env->GetStringUTFChars(ValueString, nullptr);
        FString DateString(UTF8_TO_TCHAR(ValueChars));
        Env->ReleaseStringUTFChars(ValueString, ValueChars);
        
        if (!DateString.IsEmpty())
        {
            if (!FDateTime::ParseIso8601(*DateString, ReturnValue))
            {
                UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Failed to parse ISO8601 datetime string for %s: %s"), UTF8_TO_TCHAR(MethodName), *DateString);
                ReturnValue = FDateTime();
            }
        }
        else
        {
            UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Empty datetime string returned for %s"), UTF8_TO_TCHAR(MethodName));
        }
    }
    else
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM ValueObject is not a string for method %s"), UTF8_TO_TCHAR(MethodName));
    }
    
    Env->DeleteLocalRef(ValueObject);
    return ReturnValue;
}

TArray<FString> UXRDMService::CallJNIStringArrayMethod(const char* MethodName) const
{
    TArray<FString> Result;
    
    jobject ValueObject = CallJNIResultMethod(MethodName);
    if (!ValueObject) return Result;
    
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    
    // Check if the value is a string array
    jclass ObjectArrayClass = Env->FindClass("[Ljava/lang/String;");
    jobjectArray StringArray = static_cast<jobjectArray>(ValueObject);
    jsize ArrayLength = Env->GetArrayLength(StringArray);
        
    for (jsize i = 0; i < ArrayLength; i++)
    {
        jstring StringElement = static_cast<jstring>(Env->GetObjectArrayElement(StringArray, i));
        if (StringElement)
        {
            const char* NativeString = Env->GetStringUTFChars(StringElement, nullptr);
            Result.Add(FString(UTF8_TO_TCHAR(NativeString)));
            Env->ReleaseStringUTFChars(StringElement, NativeString);
            Env->DeleteLocalRef(StringElement);
        }
        else
        {
            UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Null string element at index %d in %s"), i, UTF8_TO_TCHAR(MethodName));
        }
    }
    Env->DeleteLocalRef(ObjectArrayClass);
    
    Env->DeleteLocalRef(ValueObject);
    return Result;
}
#endif