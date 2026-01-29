#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/AbxrTypes.h"
#include "Services/Data/AbxrDataService.h"
#include "Services/Auth/AbxrAuthService.h"
#include "AbxrSubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void Authenticate() const { AuthService->Authenticate(); }
	
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void LogDebug(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Debug, Meta); }
	void LogDebug(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogDebug(Text, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void LogInfo(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Info, Meta); }
	void LogInfo(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogInfo(Text, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void LogWarn(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Warn, Meta); }
	void LogWarn(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogWarn(Text, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void LogError(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Error, Meta); }
	void LogError(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogError(Text, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void LogCritical(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Critical, Meta); }
	void LogCritical(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogCritical(Text, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta);
	void Log(const FString& Text, const ELogLevel Level)
	{
		TMap<FString, FString> Meta;
		Log(Text, Level, Meta);
	}
	void Log(const FString& Text)
	{
		TMap<FString, FString> Meta;
		Log(Text, ELogLevel::Info, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void Event(const FString& Name, TMap<FString, FString>& Meta);
	void Event(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Event(Name, Meta);
	}
	void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta);
	void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void Telemetry(const FString& Name, TMap<FString, FString>& Meta);
	void Telemetry(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Telemetry(Name, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta);
	void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta);
	void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta);
	void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta);
	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta);
	void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta);
	void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void EventCritical(const FString& Label, TMap<FString, FString>& Meta);
	void EventCritical(const FString& Label)
	{
		TMap<FString, FString> Meta;
		EventCritical(Label, Meta);
	}

	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void PresentKeyboard(const FString& PromptText, const FString& KeyboardType, const FString& EmailDomain) const;
	void PresentKeyboard(const FString& PromptText, const FString& KeyboardType) const { PresentKeyboard(PromptText, KeyboardType, FString("")); }

	// Gets the UUID assigned to device by ArborXR
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetDeviceId();

	// Gets the serial number assigned to device by OEM
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetDeviceSerial();

	// Gets the title given to device by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetDeviceTitle();

	// Gets the tags added to device by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static TArray<FString> GetDeviceTags();

	// Gets the UUID of the organization where the device is assigned. Organizations are created in the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetOrgId();

	// Gets the name assigned to organization by admin through the ArborXR Web Portal
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetOrgTitle();

	// Gets the identifier generated by ArborXR when admin assigns title to organization
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetOrgSlug();

	// Gets the physical MAC address assigned to device by OEM
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetMacAddressFixed();

	// Gets the randomized MAC address for the current WiFi connection
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetMacAddressRandom();

	// Gets whether the device is SSO authenticated
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static bool GetIsAuthenticated();

	// Gets SSO access token
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetAccessToken();

	// Gets SSO refresh token
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetRefreshToken();

	// Gets SSO token remaining lifetime
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FDateTime GetExpiresDateUtc();

	// Gets the device fingerprint
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	static FString GetFingerprint();
	
	// Start a new session with a fresh session identifier
	// Generates a new session ID and performs fresh authentication
	// Useful for starting new training experiences or resetting user context
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	void StartNewSession() const;

	// Get the learner/user data from the most recent authentication completion
	// This is the userData object from the authentication response, containing user preferences and information
	// Returns an empty map if no authentication has completed yet
	UFUNCTION(BlueprintCallable, Category = "Abxr")
	TMap<FString, FString> GetUserData() const;

	// Register a super metadata that will be automatically included in all events
	// super metadata persist across app sessions and are stored locally
	void Register(const FString& Key, const FString& Value);

	// Register a super metadata only if it doesn't already exist
	// Will not overwrite existing super metadata with the same key
	void RegisterOnce(const FString& Key, const FString& Value);
	
	// Remove a super metadata entry
	void Unregister(const FString& Key);

	// Clear all super metadata
	// Clears all super metadata from persistent storage
	void Reset();

	// Get a copy of all current super metadata
	TMap<FString, FString> GetSuperMetaData();

	void LoadSuperMetaData();
	
	FAbxrAuthService* GetAuthService() const { return AuthService.Get(); }
	FAbxrAuthService& GetAuthServiceChecked() const
	{
		check(AuthService);
		return *AuthService;
	}

private:
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);

	static void AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta);
	void Register(const FString& Key, const FString& Value, bool Overwrite);
	void SaveSuperMetaData() const;
	static bool IsReservedSuperMetaDataKey(const FString& Key);

	// Private helper function to merge super metadata and module info into metadata
	// Ensures data-specific metadata take precedence over super metadata and module info
	TMap<FString, FString> MergeSuperMetaData(TMap<FString, FString>& Meta);
	
	TSharedPtr<FAbxrAuthService> AuthService;
	TSharedPtr<FAbxrDataService> DataService;
	
	FTimerHandle AuthenticationTimerHandle;
	FDelegateHandle PostLoadMapHandle;
	bool bInitialized = false;
	
	TMap<FString, int64> AssessmentStartTimes;
	TMap<FString, int64> ObjectiveStartTimes;
	TMap<FString, int64> InteractionStartTimes;
	TMap<FString, int64> LevelStartTimes;
	FString CurrentLevel;
	
	TMap<FString, FString> SuperMetaData;
	static const FString SuperMetaDataKey;
};
