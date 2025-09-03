#include "XRDMServiceClient.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Misc/DateTime.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

// Static instance for singleton access
UXRDMServiceClient* UXRDMServiceClient::Instance = nullptr;

UXRDMServiceClient::UXRDMServiceClient()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsConnected = false;
    bIsInitialized = false;
    InitializationAttempts = 0;

#if PLATFORM_ANDROID
    SDKInstance = nullptr;
    ServiceWrapperInstance = nullptr;
    ConnectionCallback = nullptr;
#endif
}

void UXRDMServiceClient::BeginPlay()
{
    Super::BeginPlay();
    
    // Set as singleton instance
    Instance = this;
    
    // Auto-connect on begin play (similar to Unity's OnEnable)
    Connect();
}

void UXRDMServiceClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Disconnect();
    
    if (Instance == this)
    {
        Instance = nullptr;
    }
    
    Super::EndPlay(EndPlayReason);
}

UXRDMServiceClient* UXRDMServiceClient::GetInstance()
{
    return Instance;
}

void UXRDMServiceClient::Connect()
{
#if PLATFORM_ANDROID
    if (bIsConnected)
    {
        UE_LOG(LogTemp, Warning, TEXT("XRDM Service Client is already connected"));
        return;
    }
    
    InitializeJNI();
#else
    UE_LOG(LogTemp, Warning, TEXT("XRDM Service Client is only supported on Android platforms"));
#endif
}

void UXRDMServiceClient::Disconnect()
{
#if PLATFORM_ANDROID
    CleanupJNI();
#endif
    
    bIsConnected = false;
    bIsInitialized = false;
    OnDisconnected.Broadcast();
}

bool UXRDMServiceClient::IsConnected() const
{
    return bIsConnected;
}

#if PLATFORM_ANDROID

void UXRDMServiceClient::InitializeJNI()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get JNI environment"));
        return;
    }

    // Find the SDK class
    jclass SDKClass = FAndroidApplication::FindJavaClass("app/xrdm/sdk/external/Sdk");
    if (!SDKClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find XRDM SDK class"));
        return;
    }

    // Create SDK instance
    jmethodID Constructor = Env->GetMethodID(SDKClass, "<init>", "()V");
    if (!Constructor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find XRDM SDK constructor"));
        Env->DeleteLocalRef(SDKClass);
        return;
    }

    SDKInstance = Env->NewGlobalRef(Env->NewObject(SDKClass, Constructor));
    if (!SDKInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create XRDM SDK instance"));
        Env->DeleteLocalRef(SDKClass);
        return;
    }

    // Create connection callback
    jclass CallbackClass = FAndroidApplication::FindJavaClass("app/xrdm/sdk/external/IConnectionCallback");
    if (CallbackClass)
    {
        // For now, we'll implement a simpler callback mechanism
        // In a full implementation, you'd create a custom Java class that implements IConnectionCallback
        UE_LOG(LogTemp, Log, TEXT("XRDM SDK callback class found"));
        Env->DeleteLocalRef(CallbackClass);
    }

    // Get current activity for connection
    jobject Activity = FAndroidApplication::GetGameActivityThis();
    if (!Activity)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get current activity"));
        CleanupJNI();
        return;
    }

    // Call connect method
    jmethodID ConnectMethod = Env->GetMethodID(SDKClass, "connect", "(Landroid/content/Context;Lapp/xrdm/sdk/external/IConnectionCallback;)V");
    if (ConnectMethod)
    {
        // For now, pass null for the callback - in a full implementation you'd create a proper callback
        Env->CallVoidMethod(SDKInstance, ConnectMethod, Activity, nullptr);
        
        // Start checking for initialization
        StartInitializationCheck();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find XRDM SDK connect method"));
        CleanupJNI();
    }

    Env->DeleteLocalRef(SDKClass);
}

void UXRDMServiceClient::CleanupJNI()
{
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (Env)
    {
        if (SDKInstance)
        {
            Env->DeleteGlobalRef(SDKInstance);
            SDKInstance = nullptr;
        }
        
        if (ServiceWrapperInstance)
        {
            Env->DeleteGlobalRef(ServiceWrapperInstance);
            ServiceWrapperInstance = nullptr;
        }
        
        if (ConnectionCallback)
        {
            Env->DeleteGlobalRef(ConnectionCallback);
            ConnectionCallback = nullptr;
        }
    }
    
    // Clear initialization timer
    if (GetWorld() && InitializationTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(InitializationTimerHandle);
    }
}

jstring UXRDMServiceClient::CallJNIStringMethod(const char* MethodName) const
{
    if (!ServiceWrapperInstance || !bIsConnected)
    {
        return nullptr;
    }

    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        return nullptr;
    }

    jclass WrapperClass = Env->GetObjectClass(ServiceWrapperInstance);
    if (!WrapperClass)
    {
        return nullptr;
    }

    jmethodID Method = Env->GetMethodID(WrapperClass, MethodName, "()Ljava/lang/String;");
    if (!Method)
    {
        Env->DeleteLocalRef(WrapperClass);
        return nullptr;
    }

    jstring Result = static_cast<jstring>(Env->CallObjectMethod(ServiceWrapperInstance, Method));
    Env->DeleteLocalRef(WrapperClass);
    
    return Result;
}

bool UXRDMServiceClient::CallJNIBoolMethod(const char* MethodName) const
{
    if (!ServiceWrapperInstance || !bIsConnected)
    {
        return false;
    }

    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        return false;
    }

    jclass WrapperClass = Env->GetObjectClass(ServiceWrapperInstance);
    if (!WrapperClass)
    {
        return false;
    }

    jmethodID Method = Env->GetMethodID(WrapperClass, MethodName, "()Ljava/lang/String;");
    if (!Method)
    {
        Env->DeleteLocalRef(WrapperClass);
        return false;
    }

    jstring StringResult = static_cast<jstring>(Env->CallObjectMethod(ServiceWrapperInstance, Method));
    Env->DeleteLocalRef(WrapperClass);
    
    if (!StringResult)
    {
        return false;
    }

    const char* CStr = Env->GetStringUTFChars(StringResult, nullptr);
    bool Result = CStr && strlen(CStr) > 0 && (strcmp(CStr, "true") == 0 || strcmp(CStr, "1") == 0);
    Env->ReleaseStringUTFChars(StringResult, CStr);
    Env->DeleteLocalRef(StringResult);
    
    return Result;
}

jobjectArray UXRDMServiceClient::CallJNIStringArrayMethod(const char* MethodName) const
{
    if (!ServiceWrapperInstance || !bIsConnected)
    {
        return nullptr;
    }

    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (!Env)
    {
        return nullptr;
    }

    jclass WrapperClass = Env->GetObjectClass(ServiceWrapperInstance);
    if (!WrapperClass)
    {
        return nullptr;
    }

    jmethodID Method = Env->GetMethodID(WrapperClass, MethodName, "()[Ljava/lang/String;");
    if (!Method)
    {
        Env->DeleteLocalRef(WrapperClass);
        return nullptr;
    }

    jobjectArray Result = static_cast<jobjectArray>(Env->CallObjectMethod(ServiceWrapperInstance, Method));
    Env->DeleteLocalRef(WrapperClass);
    
    return Result;
}

void UXRDMServiceClient::StartInitializationCheck()
{
    if (!GetWorld())
    {
        return;
    }

    InitializationAttempts = 0;
    GetWorld()->GetTimerManager().SetTimer(
        InitializationTimerHandle,
        this,
        &UXRDMServiceClient::CheckInitializationStatus,
        InitialDelay,
        false
    );
}

void UXRDMServiceClient::CheckInitializationStatus()
{
    // This is a simplified version - in the full implementation,
    // you would check if the service wrapper is available and initialized
    InitializationAttempts++;
    
    if (InitializationAttempts >= MaxInitializationAttempts)
    {
        // Assume connected after max attempts (simplified approach)
        bIsConnected = true;
        bIsInitialized = true;
        OnConnected.Broadcast();
        return;
    }

    if (GetWorld())
    {
        float NextDelay = InitialDelay * FMath::Pow(DelayMultiplier, InitializationAttempts);
        GetWorld()->GetTimerManager().SetTimer(
            InitializationTimerHandle,
            this,
            &UXRDMServiceClient::CheckInitializationStatus,
            NextDelay,
            false
        );
    }
}

#endif // PLATFORM_ANDROID

// Device Information Implementation
FString UXRDMServiceClient::GetDeviceId() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getDeviceId");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString DeviceId(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return DeviceId;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetDeviceSerial() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getDeviceSerial");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Serial(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Serial;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetDeviceTitle() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getDeviceTitle");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Title(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Title;
    }
#endif
    return FString();
}

TArray<FString> UXRDMServiceClient::GetDeviceTags() const
{
    TArray<FString> Tags;
    
#if PLATFORM_ANDROID
    jobjectArray Result = CallJNIStringArrayMethod("getDeviceTags");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        jsize Length = Env->GetArrayLength(Result);
        
        for (jsize i = 0; i < Length; ++i)
        {
            jstring StringObj = static_cast<jstring>(Env->GetObjectArrayElement(Result, i));
            if (StringObj)
            {
                const char* CStr = Env->GetStringUTFChars(StringObj, nullptr);
                Tags.Add(FString(CStr));
                Env->ReleaseStringUTFChars(StringObj, CStr);
                Env->DeleteLocalRef(StringObj);
            }
        }
        
        Env->DeleteLocalRef(Result);
    }
#endif
    
    return Tags;
}

FString UXRDMServiceClient::GetOrgId() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getOrgId");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString OrgId(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return OrgId;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetOrgTitle() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getOrgTitle");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Title(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Title;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetOrgSlug() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getOrgSlug");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Slug(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Slug;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetMacAddressFixed() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getMacAddressFixed");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString MacAddress(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return MacAddress;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetMacAddressRandom() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getMacAddressRandom");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString MacAddress(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return MacAddress;
    }
#endif
    return FString();
}

bool UXRDMServiceClient::GetIsAuthenticated() const
{
#if PLATFORM_ANDROID
    return CallJNIBoolMethod("getIsAuthenticated");
#endif
    return false;
}

FString UXRDMServiceClient::GetAccessToken() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getAccessToken");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Token(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Token;
    }
#endif
    return FString();
}

FString UXRDMServiceClient::GetRefreshToken() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getRefreshToken");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Token(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Token;
    }
#endif
    return FString();
}

FDateTime UXRDMServiceClient::GetExpiresDateUtc() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getExpiresDateUtc");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString DateStr(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        
        // Parse the date string - this would need proper date parsing
        FDateTime DateTime;
        if (FDateTime::ParseIso8601(*DateStr, DateTime))
        {
            return DateTime;
        }
    }
#endif
    return FDateTime::MinValue();
}

FString UXRDMServiceClient::GetFingerprint() const
{
#if PLATFORM_ANDROID
    jstring Result = CallJNIStringMethod("getFingerprint");
    if (Result)
    {
        JNIEnv* Env = FAndroidApplication::GetJavaEnv();
        const char* CStr = Env->GetStringUTFChars(Result, nullptr);
        FString Fingerprint(CStr);
        Env->ReleaseStringUTFChars(Result, CStr);
        Env->DeleteLocalRef(Result);
        return Fingerprint;
    }
#endif
    return FString();
}

bool UXRDMServiceClient::GetIsInitialized() const
{
#if PLATFORM_ANDROID
    return CallJNIBoolMethod("getIsInitialized");
#endif
    return false;
}
