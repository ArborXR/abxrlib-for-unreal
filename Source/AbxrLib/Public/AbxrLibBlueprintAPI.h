#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/AbxrPublicTypes.h"
#include "AbxrLibBlueprintAPI.generated.h"

class UAbxrSubsystem;

UCLASS(BlueprintType)
class ABXRLIB_API UAbxrLibBlueprintEvents : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Abxr|Auth")
	FAbxrAuthCompleted OnAuthCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Abxr|UI")
	FAbxrPopupShown OnPopupShown;

	UPROPERTY(BlueprintAssignable, Category = "Abxr|UI")
	FAbxrPopupHidden OnPopupHidden;

	UPROPERTY(BlueprintAssignable, Category = "Abxr|Modules")
	FAbxrModuleTarget OnModuleTarget;

	UPROPERTY(BlueprintAssignable, Category = "Abxr|Modules")
	FAbxrAllModulesCompleted OnAllModulesCompleted;

	void TryBindToActiveSubsystem();

private:
	UFUNCTION()
	void HandleAuthCompleted(const bool bSuccess);

	UFUNCTION()
	void HandlePopupShown();

	UFUNCTION()
	void HandlePopupHidden();

	UFUNCTION()
	void HandleModuleTarget(const FString& ModuleTarget);

	UFUNCTION()
	void HandleAllModulesCompleted();

	TWeakObjectPtr<UAbxrSubsystem> BoundSubsystem;
};

UCLASS()
class ABXRLIB_API UAbxrLibBlueprintAPI : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Abxr", meta = (DisplayName = "Get Abxr Lib Events"))
	static UAbxrLibBlueprintEvents* GetAbxrLibEvents();

	UFUNCTION(BlueprintCallable, Category = "Abxr|Auth")
	static void Authenticate();

	UFUNCTION(BlueprintPure, Category = "Abxr|UI")
	static bool IsPopupVisible();

	UFUNCTION(BlueprintPure, Category = "Abxr|Modules")
	static TArray<FAbxrModuleData> GetModuleList();

	UFUNCTION(BlueprintCallable, Category = "Abxr|Modules")
	static bool StartModuleAtIndex(int32 ModuleIndex);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void LogDebug(const FString& Text, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void LogInfo(const FString& Text, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void LogWarn(const FString& Text, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void LogError(const FString& Text, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void LogCritical(const FString& Text, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void Log(const FString& Text, ELogLevel Level, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void Event(const FString& Name, const FVector& Position, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void Telemetry(const FString& Name, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventAssessmentStart(const FString& AssessmentName, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventAssessmentComplete(const FString& AssessmentName, int32 Score, EEventStatus Status, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventObjectiveStart(const FString& ObjectiveName, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventObjectiveComplete(const FString& ObjectiveName, int32 Score, EEventStatus Status, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventInteractionStart(const FString& InteractionName, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventLevelStart(const FString& LevelName, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventLevelComplete(const FString& LevelName, int32 Score, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintCallable, Category = "Abxr", meta = (AutoCreateRefTerm = "Meta"))
	static void EventCritical(const FString& Label, const TMap<FString, FString>& Meta);

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetDeviceId();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetDeviceSerial();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetDeviceTitle();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static TArray<FString> GetDeviceTags();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetOrgId();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetOrgTitle();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetOrgSlug();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetMacAddressFixed();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetMacAddressRandom();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static bool GetIsAuthenticated();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetAccessToken();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetRefreshToken();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FDateTime GetExpiresDateUtc();

	UFUNCTION(BlueprintPure, Category = "Abxr|XRDM")
	static FString GetFingerprint();

	UFUNCTION(BlueprintCallable, Category = "Abxr|Session")
	static void StartNewSession();

	UFUNCTION(BlueprintPure, Category = "Abxr|Auth")
	static TMap<FString, FString> GetUserData();

	UFUNCTION(BlueprintCallable, Category = "Abxr|MetaData")
	static void Register(const FString& Key, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Abxr|MetaData")
	static void RegisterOnce(const FString& Key, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Abxr|MetaData")
	static void Unregister(const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Abxr|MetaData")
	static void Reset();

	UFUNCTION(BlueprintPure, Category = "Abxr|MetaData")
	static TMap<FString, FString> GetSuperMetaData();

	UFUNCTION(BlueprintCallable, Category = "Abxr|MetaData")
	static void LoadSuperMetaData();
};
