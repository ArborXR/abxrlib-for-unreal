#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Engine/TimerHandle.h"
#include "Misc/DateTime.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#include "XRDMServiceClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnXRDMConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnXRDMDisconnected);

/**
 * XRDM Service Client for communicating with ArborXR device management services on Android VR headsets.
 * This is the Unreal equivalent of the Unity ArborServiceClient.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(AbxrLib), meta=(BlueprintSpawnableComponent))
class ABXRLIB_API UXRDMServiceClient : public UActorComponent
{
    GENERATED_BODY()

public:
    UXRDMServiceClient();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "XRDM")
    FOnXRDMConnected OnConnected;

    UPROPERTY(BlueprintAssignable, Category = "XRDM")
    FOnXRDMDisconnected OnDisconnected;

    // Connection Management
    UFUNCTION(BlueprintCallable, Category = "XRDM")
    void Connect();

    UFUNCTION(BlueprintCallable, Category = "XRDM")
    void Disconnect();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM")
    bool IsConnected() const;

    // Device Information Methods
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Device")
    FString GetDeviceId() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Device")
    FString GetDeviceSerial() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Device")
    FString GetDeviceTitle() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Device")
    TArray<FString> GetDeviceTags() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Organization")
    FString GetOrgId() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Organization")
    FString GetOrgTitle() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Organization")
    FString GetOrgSlug() const;

    // Network Information
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Network")
    FString GetMacAddressFixed() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Network")
    FString GetMacAddressRandom() const;

    // Authentication Information
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Auth")
    bool GetIsAuthenticated() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Auth")
    FString GetAccessToken() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Auth")
    FString GetRefreshToken() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|Auth")
    FDateTime GetExpiresDateUtc() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|System")
    FString GetFingerprint() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM|System")
    bool GetIsInitialized() const;

    // Static instance access (similar to Unity's singleton pattern)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "XRDM")
    static UXRDMServiceClient* GetInstance();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    static UXRDMServiceClient* Instance;

#if PLATFORM_ANDROID
    // JNI Objects
    jobject SDKInstance;
    jobject ServiceWrapperInstance;
    jobject ConnectionCallback;
    
    // JNI Helper Methods
    void InitializeJNI();
    void CleanupJNI();
    jstring CallJNIStringMethod(const char* MethodName) const;
    bool CallJNIBoolMethod(const char* MethodName) const;
    jobjectArray CallJNIStringArrayMethod(const char* MethodName) const;
    
    // Connection Callback
    void OnNativeConnected(jobject ServiceWrapper);
    static void Java_OnConnected(JNIEnv* Env, jobject Thiz, jlong NativePtr, jobject ServiceWrapper);
#endif

    bool bIsConnected;
    bool bIsInitialized;
    
    // Async initialization handling
    void StartInitializationCheck();
    void CheckInitializationStatus();
    
    FTimerHandle InitializationTimerHandle;
    int32 InitializationAttempts;
    static constexpr int32 MaxInitializationAttempts = 7;
    static constexpr float InitialDelay = 0.5f;
    static constexpr float DelayMultiplier = 1.5f;
};
