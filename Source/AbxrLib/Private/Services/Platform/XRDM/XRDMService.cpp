#include "XRDMService.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Async/AsyncWork.h"
#include "Types/AbxrLog.h"
#include "UObject/Package.h"

UXRDMService* UXRDMService::Instance = nullptr;

UXRDMService* UXRDMService::GetInstance()
{
    if (!Instance)
    {
        // Create instance in the transient package to prevent GC issues
        Instance = NewObject<UXRDMService>(GetTransientPackage(), StaticClass());
        Instance->AddToRoot(); // Prevent garbage collection
    }
    
    return Instance;
}

void UXRDMService::DestroyInstance()
{
    if (Instance)
    {
        Instance->Shutdown();
        Instance->RemoveFromRoot();
        Instance = nullptr;
    }
}

UXRDMService::UXRDMService()
{
    // Private constructor - can only be called by GetInstance()
}

void UXRDMService::BeginDestroy()
{
    if (bIsInitialized) Shutdown();
    Super::BeginDestroy();
}

void UXRDMService::Initialize()
{
    if (bIsInitialized) return;
    
    bIsInitialized = true;
    bConnectionAttemptComplete = false; // Reset connection state
    
#if PLATFORM_ANDROID
    // Register native methods for the bridge
    RegisterNativeMethods();
    InitializeSDK();
#endif
}

void UXRDMService::Shutdown()
{
    if (!bIsInitialized) return;
    
    bIsInitialized = false;
    bConnectionAttemptComplete = false;
    
    // Clear timeout ticker
    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }
    
    // Complete any pending promises with failure
    for (auto& Promise : PendingConnectionPromises)
    {
        if (Promise.IsValid())
        {
            Promise->SetValue(false);
        }
    }
    PendingConnectionPromises.Empty();
    
#if PLATFORM_ANDROID
    CleanupJNI();
#endif
}

TSharedPtr<TPromise<bool>> UXRDMService::WaitForConnection()
{
    auto Promise = MakeShared<TPromise<bool>>();
    
    if (!bIsInitialized)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDMService not initialized, connection will fail"));
        Promise->SetValue(false);
        return Promise;
    }
    
    if (bConnectionAttemptComplete)
    {
        Promise->SetValue(bIsConnected);
        return Promise;
    }
    
    // Connection attempt in progress, add to pending list
    PendingConnectionPromises.Add(Promise);
    
    if (!ConnectionTimeoutHandle.IsValid())
    {
        ConnectionTimeoutHandle = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UXRDMService::OnConnectionTimeout), 
            1.0f // 1 second timeout
        );
    }
    
    return Promise;
}

bool UXRDMService::OnConnectionTimeout(float DeltaTime)
{
    if (!bConnectionAttemptComplete)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("XRDM connection attempt timed out after 1 second"));
        CompleteConnectionAttempt(false);
    }
    
    return false;
}

bool UXRDMService::IsConnectionAttemptComplete() const
{
    return bConnectionAttemptComplete;
}

void UXRDMService::CompleteConnectionAttempt(bool bSuccess)
{
    bConnectionAttemptComplete = true;
    bIsConnected = bSuccess;
    
    // Clear the timeout ticker if it exists
    if (ConnectionTimeoutHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(ConnectionTimeoutHandle);
        ConnectionTimeoutHandle.Reset();
    }
    
    // Complete all pending promises
    for (auto& Promise : PendingConnectionPromises)
    {
        if (Promise.IsValid())
        {
            Promise->SetValue(bSuccess);
        }
    }
    PendingConnectionPromises.Empty();
}

bool UXRDMService::IsConnected()
{
    if (!Instance) return false;
    
#if PLATFORM_ANDROID
    return Instance->bIsConnected && Instance->ServiceWrapper != nullptr;
#else
    return false;
#endif
}

FString UXRDMService::GetDeviceId()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getDeviceId");
#else
    return FString();
#endif
}

FString UXRDMService::GetDeviceSerial()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getDeviceSerial");
#else
    return FString();
#endif
}

FString UXRDMService::GetDeviceTitle()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getDeviceTitle");
#else
    return FString();
#endif
}

TArray<FString> UXRDMService::GetDeviceTags()
{
    if (!IsConnected()) return TArray<FString>();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIStringArrayMethod("getDeviceTags");
#else
    return TArray<FString>();
#endif
}

FString UXRDMService::GetOrgId()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getOrgId");
#else
    return FString();
#endif
}

FString UXRDMService::GetOrgTitle()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getOrgTitle");
#else
    return FString();
#endif
}

FString UXRDMService::GetOrgSlug()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getOrgSlug");
#else
    return FString();
#endif
}

FString UXRDMService::GetMacAddressFixed()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getMacAddressFixed");
#else
    return FString();
#endif
}

FString UXRDMService::GetMacAddressRandom()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getMacAddressRandom");
#else
    return FString();
#endif
}

bool UXRDMService::GetIsAuthenticated()
{
    if (!IsConnected()) return false;
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIBoolMethod("getIsAuthenticated");
#else
    return false;
#endif
}

FString UXRDMService::GetAccessToken()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getAccessToken");
#else
    return FString();
#endif
}

FString UXRDMService::GetRefreshToken()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getRefreshToken");
#else
    return FString();
#endif
}

FDateTime UXRDMService::GetExpiresDateUtc()
{
    if (!IsConnected()) return FDateTime();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIDateTimeMethod("getExpiresDateUtc");
#else
    return FDateTime();
#endif
}

bool UXRDMService::GetIsInitialized()
{
    if (!IsConnected()) return false;
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIBoolMethod("getIsInitialized");
#else
    return false;
#endif
}

FString UXRDMService::GetFingerprint()
{
    if (!IsConnected()) return FString();
    
#if PLATFORM_ANDROID
    UXRDMService* LocalInstance = GetInstance();
    return LocalInstance->CallJNIResultString("getFingerprint");
#else
    return FString();
#endif
}

#if PLATFORM_ANDROID

void UXRDMService::RegisterNativeMethods()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to get JNI environment for native method registration"));
        return;
    }
    
    jclass CallbackClass = FAndroidApplication::FindJavaClass("com.xrdm.xrdmbridge.NativeConnectionCallback");
    if (!CallbackClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find NativeConnectionCallback class for native method registration"));
        return;
    }

    // Define the native methods (both onConnected and onDisconnected)
    JNINativeMethod NativeMethods[] = {
        {"nativeOnConnected", "(Ljava/lang/Object;)V", (void*)&UXRDMService::NativeOnConnected},
        {"nativeOnDisconnected", "(Z)V", (void*)&UXRDMService::NativeOnDisconnected}
    };
    
    int Result = Env->RegisterNatives(CallbackClass, NativeMethods, 2);
    if (Result < 0)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to register native methods"));
    }
    else
    {
        UE_LOG(LogAbxrLib, Log, TEXT("Successfully registered XRDM native methods (onConnected & onDisconnected)"));
    }
}

void JNICALL UXRDMService::NativeOnConnected(JNIEnv* Env, jclass Clazz, jobject Service)
{
    UXRDMService* LocalInstance = UXRDMService::GetInstance();
    if (LocalInstance && Service)
    {
        LocalInstance->ServiceWrapper = Env->NewGlobalRef(Service);
        LocalInstance->bIsConnected = true;
        
        // Notify on the game thread
        AsyncTask(ENamedThreads::GameThread, [LocalInstance]()
        {
            if (LocalInstance && LocalInstance->bIsInitialized)
            {
                LocalInstance->CompleteConnectionAttempt(true);
                UE_LOG(LogAbxrLib, Log, TEXT("XRDM SDK connected via native callback"));
            }
        });
    }
}

void JNICALL UXRDMService::NativeOnDisconnected(JNIEnv* Env, jclass Clazz, jboolean WasClean)
{
    UXRDMService* LocalInstance = UXRDMService::GetInstance();
    if (LocalInstance)
    {
        // Clear the service reference
        if (LocalInstance->ServiceWrapper)
        {
            Env->DeleteGlobalRef(LocalInstance->ServiceWrapper);
            LocalInstance->ServiceWrapper = nullptr;
        }
        LocalInstance->bIsConnected = false;
    }
}

void UXRDMService::InitializeSDK()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to get JNI environment"));
        CompleteConnectionAttempt(false);
        return;
    }

    SdkClass = FAndroidApplication::FindJavaClass("app.xrdm.sdk.external.Sdk");
    if (!SdkClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find any SDK class"));
        CompleteConnectionAttempt(false);
        return;
    }

    jmethodID Constructor = Env->GetMethodID(SdkClass, "<init>", "()V");
    if (!Constructor)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find SDK constructor"));
        CompleteConnectionAttempt(false);
        return;
    }

    SdkInstance = Env->NewObject(SdkClass, Constructor);
    if (!SdkInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to create SDK instance"));
        CompleteConnectionAttempt(false);
        return;
    }

    SdkInstance = Env->NewGlobalRef(SdkInstance);
    UE_LOG(LogAbxrLib, Log, TEXT("SDK instance created successfully"));
    
    ConnectToService();
}

void UXRDMService::ConnectToService()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env || !SdkInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("JNI environment or SDK instance not available"));
        CompleteConnectionAttempt(false);
        return;
    }

    jobject CurrentActivity = FAndroidApplication::GetGameActivityThis();
    if (!CurrentActivity)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to get current activity"));
        CompleteConnectionAttempt(false);
        return;
    }
    
    jclass CallbackClass = FAndroidApplication::FindJavaClass("com.xrdm.xrdmbridge.NativeConnectionCallback");
    if (!CallbackClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find NativeConnectionCallback class"));
        CompleteConnectionAttempt(false);
        return;
    }
    
    jmethodID CallbackConstructor = Env->GetMethodID(CallbackClass, "<init>", "()V");
    if (!CallbackConstructor)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find NativeConnectionCallback constructor"));
        CompleteConnectionAttempt(false);
        return;
    }
    
    jobject CallbackInstance = Env->NewObject(CallbackClass, CallbackConstructor);
    if (!CallbackInstance)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to create NativeConnectionCallback instance"));
        CompleteConnectionAttempt(false);
        return;
    }
    
    // Keep a global reference to the callback
    ConnectionCallback = Env->NewGlobalRef(CallbackInstance);
    
    // Try connect with Context + IConnectionCallback
    jmethodID ConnectMethod = Env->GetMethodID(SdkClass, "connect", "(Landroid/content/Context;Lapp/xrdm/sdk/external/IConnectionCallback;)V");
    if (ConnectMethod)
    {
        UE_LOG(LogAbxrLib, Log, TEXT("Found connect method with Context + IConnectionCallback signature"));
        Env->CallVoidMethod(SdkInstance, ConnectMethod, CurrentActivity, ConnectionCallback);
        return;
    }
    
    // Try with Activity instead of Context
    ConnectMethod = Env->GetMethodID(SdkClass, "connect", "(Landroid/app/Activity;Lapp/xrdm/sdk/external/IConnectionCallback;)V");
    if (ConnectMethod)
    {
        UE_LOG(LogAbxrLib, Log, TEXT("Found connect method with Activity + IConnectionCallback signature"));
        Env->CallVoidMethod(SdkInstance, ConnectMethod, CurrentActivity, ConnectionCallback);
        return;
    }
    
    // Clean up callback if we couldn't use it
    Env->DeleteGlobalRef(ConnectionCallback);
    ConnectionCallback = nullptr;
    
    UE_LOG(LogAbxrLib, Error, TEXT("Failed to find connect method with IConnectionCallback"));
    CompleteConnectionAttempt(false);
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
        UE_LOG(LogAbxrLib, Warning, TEXT("Cannot call %s - service not connected"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }

    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("No JNI environment for %s"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }

    // Clear any pending exceptions first
    if (Env->ExceptionCheck())
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Clearing pending exception before calling %s"), UTF8_TO_TCHAR(MethodName));
        Env->ExceptionClear();
    }

    jclass ServiceWrapperClass = Env->GetObjectClass(ServiceWrapper);
    if (!ServiceWrapperClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to get service wrapper class for %s"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jmethodID Method = Env->GetMethodID(ServiceWrapperClass, MethodName, "()Lapp/xrdm/sdk/external/Result;");
    if (!Method)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Failed to find method %s with Result signature"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jobject ResultObject = Env->CallObjectMethod(ServiceWrapper, Method);
    
    if (Env->ExceptionCheck())
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Exception occurred calling %s"), UTF8_TO_TCHAR(MethodName));
        Env->ExceptionDescribe();
        Env->ExceptionClear();
        return nullptr;
    }
    
    if (!ResultObject)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Method %s returned null Result object"), UTF8_TO_TCHAR(MethodName));
        return nullptr;
    }
    
    jclass ResultClass = FAndroidApplication::FindJavaClass("app.xrdm.sdk.external.Result");
    if (!ResultClass)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Could not find Result class"));
        Env->DeleteLocalRef(ResultObject);
        return nullptr;
    }
    
    jmethodID IsOkMethod = Env->GetMethodID(ResultClass, "isOk", "()Z");
    if (!IsOkMethod)
    {
        UE_LOG(LogAbxrLib, Error, TEXT("Could not find isOk method on Result class"));
        Env->DeleteLocalRef(ResultObject);
        return nullptr;
    }
    
    jboolean IsOk = Env->CallBooleanMethod(ResultObject, IsOkMethod);
    
    if (IsOk == JNI_TRUE)
    {
        jmethodID GetValueMethod = Env->GetMethodID(ResultClass, "getValue", "()Ljava/lang/Object;");
        if (!GetValueMethod)
        {
            UE_LOG(LogAbxrLib, Error, TEXT("Could not find getValue method on Result class"));
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
                UE_LOG(LogAbxrLib, Error, TEXT("SDK Error calling %s: %s"), UTF8_TO_TCHAR(MethodName), UTF8_TO_TCHAR(ErrorChars));
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
    
    UE_LOG(LogAbxrLib, Warning, TEXT("getValue() returned non-string for %s"), UTF8_TO_TCHAR(MethodName));
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
        UE_LOG(LogAbxrLib, Warning, TEXT("ValueObject is not a String for method %s"), UTF8_TO_TCHAR(MethodName));
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
                UE_LOG(LogAbxrLib, Warning, TEXT("Failed to parse ISO8601 datetime string for %s: %s"), UTF8_TO_TCHAR(MethodName), *DateString);
                ReturnValue = FDateTime();
            }
        }
        else
        {
            UE_LOG(LogAbxrLib, Warning, TEXT("Empty datetime string returned for %s"), UTF8_TO_TCHAR(MethodName));
        }
    }
    else
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("ValueObject is not a string for method %s"), UTF8_TO_TCHAR(MethodName));
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
            UE_LOG(LogAbxrLib, Warning, TEXT("Null string element at index %d in %s"), i, UTF8_TO_TCHAR(MethodName));
        }
    }
    Env->DeleteLocalRef(ObjectArrayClass);
    
    Env->DeleteLocalRef(ValueObject);
    return Result;
}
#endif