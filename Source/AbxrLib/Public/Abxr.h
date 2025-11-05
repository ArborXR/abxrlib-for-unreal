#pragma once

#include "CoreMinimal.h"
#include "Authentication.h"
#include "Engine/World.h"
#include "UObject/WeakObjectPtr.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abxr.generated.h"

UENUM(BlueprintType)
enum class EEventStatus : uint8
{
	Pass        UMETA(DisplayName = "pass"),
	Fail        UMETA(DisplayName = "fail"),
	Complete    UMETA(DisplayName = "complete"),
	Incomplete  UMETA(DisplayName = "incomplete"),
	Browsed     UMETA(DisplayName = "browsed")
};

UENUM(BlueprintType)
enum class ELogLevel : uint8
{
	Debug     UMETA(DisplayName = "debug"),
	Info      UMETA(DisplayName = "info"),
	Warn      UMETA(DisplayName = "warn"),
	Error     UMETA(DisplayName = "error"),
	Critical  UMETA(DisplayName = "critical")
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Bool         UMETA(DisplayName = "bool"),
	Select       UMETA(DisplayName = "select"),
	Text         UMETA(DisplayName = "text"),
	Rating       UMETA(DisplayName = "rating"),
	Number       UMETA(DisplayName = "number"),
	Matching     UMETA(DisplayName = "matching"),
	Performance  UMETA(DisplayName = "performance"),
	Sequencing   UMETA(DisplayName = "sequencing")
};

UCLASS()
class ABXRLIB_API UAbxr : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SetWorld(UWorld* World) { GWorldWeak = World; }
	
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Authenticate() { Authentication::Authenticate(); }
	
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogDebug(const FString& Text, const TMap<FString, FString>& Meta);
	static void LogDebug(const FString& Text) { LogDebug(Text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogInfo(const FString& Text, const TMap<FString, FString>& Meta);
	static void LogInfo(const FString& Text) { LogInfo(Text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogWarn(const FString& Text, const TMap<FString, FString>& Meta);
	static void LogWarn(const FString& Text) { LogWarn(Text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogError(const FString& Text, const TMap<FString, FString>& Meta);
	static void LogError(const FString& Text) { LogError(Text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogCritical(const FString& Text, const TMap<FString, FString>& Meta);
	static void LogCritical(const FString& Text) { LogCritical(Text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Log(const FString& Text, ELogLevel Level, TMap<FString, FString> Meta);
	static void Log(const FString& Text, ELogLevel Level) { Log(Text, Level, TMap<FString, FString>()); }
	static void Log(const FString& Text) { Log(Text, ELogLevel::Info, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Event(const FString& Name, TMap<FString, FString> Meta);
	static void Event(const FString& Name) { Event(Name, TMap<FString, FString>()); }
	static void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta);
	static void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Telemetry(const FString& Name, TMap<FString, FString> Meta);
	static void Telemetry(const FString& Name) { Telemetry(Name, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta);
	static void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	static void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta);
	static void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	static void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta);
	static void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta);
	static void EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta);
	static void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta);
	static void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void EventCritical(const FString& Label, const TMap<FString, FString>& Meta);
	static void EventCritical(const FString& Label) { EventCritical(Label, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void PresentKeyboard(const FString& PromptText, const FString& KeyboardType, const FString& EmailDomain);
	static void PresentKeyboard(const FString& PromptText, const FString& KeyboardType) { PresentKeyboard(PromptText, KeyboardType, FString("")); }

	// Gets the UUID assigned to device by ArborXR
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetDeviceId();

	// Gets the serial number assigned to device by OEM
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetDeviceSerial();

	// Gets the title given to device by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetDeviceTitle();

	// Gets the tags added to device by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static TArray<FString> GetDeviceTags();

	// Gets the UUID of the organization where the device is assigned. Organizations are created in the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetOrgId();

	// Gets the name assigned to organization by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetOrgTitle();

	// Gets the identifier generated by ArborXR when admin assigns title to organization
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetOrgSlug();

	// Gets the physical MAC address assigned to device by OEM
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetMacAddressFixed();

	// Gets the randomized MAC address for the current WiFi connection
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetMacAddressRandom();

	// Gets whether the device is SSO authenticated
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static bool GetIsAuthenticated();

	// Gets SSO access token
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetAccessToken();

	// Gets SSO refresh token
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetRefreshToken();

	// Gets SSO token remaining lifetime
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FDateTime GetExpiresDateUtc();

	// Gets the device fingerprint
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static FString GetFingerprint();

private:
	static TMap<FString, int64> AssessmentStartTimes;
	static TMap<FString, int64> ObjectiveStartTimes;
	static TMap<FString, int64> InteractionStartTimes;
	static TMap<FString, int64> LevelStartTimes;
	static TWeakObjectPtr<UWorld> GWorldWeak;

	static void AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta);
};
