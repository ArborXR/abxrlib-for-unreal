#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/AbxrTypes.h"
#include "Services/Data/AbxrDataService.h"
#include "Services/Platform/XRDM/XRDMService.h"
#include "Services/Auth/AbxrAuthService.h"
#include "AbxrSubsystem.generated.h"

class UAbxrUISubsystem;

UCLASS()
class ABXRLIB_API UAbxrSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UAbxrUISubsystem* GetUISubsystem() const;
	void SubmitResponse(const FString& Response, const FAbxrInputRequest& InputRequest);
	
	TFunction<void(const FAbxrInputRequest&)> OnInputRequested;
	FAbxrAuthCompleted OnAuthCompleted;
	
	FAbxrModuleTarget OnModuleTarget;
	FAbxrAllModulesCompleted OnAllModulesCompleted;
	
	TArray<FAbxrModuleData> GetModuleList() const { return AuthService->GetAuthResponse().Modules; }
	
	bool StartModuleAtIndex(const int ModuleIndex);

	void Authenticate() const;
	
	void PollUser(const FString& Prompt, const EPollType PollType, const TArray<FString>& Responses) const;
	void PollUser(const FString& Prompt, const EPollType PollType) const
	{
		const TArray<FString> Responses;
		PollUser(Prompt, PollType, Responses);
	}
	
	void LogDebug(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Debug, Meta); }
	void LogDebug(const FString& Text) { TMap<FString, FString> Meta; LogDebug(Text, Meta); }

	void LogInfo(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Info, Meta); }
	void LogInfo(const FString& Text) { TMap<FString, FString> Meta; LogInfo(Text, Meta); }

	void LogWarn(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Warn, Meta); }
	void LogWarn(const FString& Text) { TMap<FString, FString> Meta; LogWarn(Text, Meta); }

	void LogError(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Error, Meta); }
	void LogError(const FString& Text) { TMap<FString, FString> Meta; LogError(Text, Meta); }

	void LogCritical(const FString& Text, TMap<FString, FString>& Meta) { Log(Text, ELogLevel::Critical, Meta); }
	void LogCritical(const FString& Text) { TMap<FString, FString> Meta; LogCritical(Text, Meta); }

	void Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta);
	void Log(const FString& Text, const ELogLevel Level) { TMap<FString, FString> Meta; Log(Text, Level, Meta); }
	void Log(const FString& Text) { TMap<FString, FString> Meta; Log(Text, ELogLevel::Info, Meta); }

	void Event(const FString& Name, TMap<FString, FString>& Meta);
	void Event(const FString& Name) { TMap<FString, FString> Meta; Event(Name, Meta); }
	void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta);
	void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}

	void Telemetry(const FString& Name, TMap<FString, FString>& Meta);
	void Telemetry(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Telemetry(Name, Meta);
	}

	void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta);
	void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}

	void EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}

	void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta);
	void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}

	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta);
	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}

	void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta);
	void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}

	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta);
	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}

	void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta);
	void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}

	void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta);
	void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}

	void EventCritical(const FString& Label, TMap<FString, FString>& Meta);
	void EventCritical(const FString& Label)
	{
		TMap<FString, FString> Meta;
		EventCritical(Label, Meta);
	}
	
	// XRDM Functions
	FString GetDeviceId() const { return XRDMService ? XRDMService->GetDeviceId() : TEXT(""); }
	FString GetDeviceSerial() const { return XRDMService ? XRDMService->GetDeviceSerial() : TEXT(""); }
	FString GetDeviceTitle() const { return XRDMService ? XRDMService->GetDeviceTitle() : TEXT(""); }
	TArray<FString> GetDeviceTags() const { return XRDMService ? XRDMService->GetDeviceTags() : TArray<FString>(); }
	FString GetOrgId() const { return XRDMService ? XRDMService->GetOrgId() : TEXT(""); }
	FString GetOrgTitle() const { return XRDMService ? XRDMService->GetOrgTitle() : TEXT(""); }
	FString GetOrgSlug() const { return XRDMService ? XRDMService->GetOrgSlug() : TEXT(""); }
	FString GetMacAddressFixed() const { return XRDMService ? XRDMService->GetMacAddressFixed() : TEXT(""); }
	FString GetMacAddressRandom() const { return XRDMService ? XRDMService->GetMacAddressRandom() : TEXT(""); }
	bool GetIsAuthenticated() const { return XRDMService ? XRDMService->GetIsAuthenticated() : false; }
	FString GetAccessToken() const { return XRDMService ? XRDMService->GetAccessToken() : TEXT(""); }
	FString GetRefreshToken() const { return XRDMService ? XRDMService->GetRefreshToken() : TEXT(""); }
	FDateTime GetExpiresDateUtc() const { return XRDMService ? XRDMService->GetExpiresDateUtc() : FDateTime::MinValue(); }
	FString GetFingerprint() const { return XRDMService ? XRDMService->GetFingerprint() : TEXT(""); }
	
	void StartNewSession();
	
	TMap<FString, FString> GetUserData() const { return AuthService->GetAuthResponse().UserData; }
	
	void Register(const FString& Key, const FString& Value) { Register(Key, Value, true); }
	void RegisterOnce(const FString& Key, const FString& Value) { Register(Key, Value, false); }
	void Unregister(const FString& Key);
	void Reset();
	
	TMap<FString, FString> GetSuperMetaData() { return SuperMetaData; }

	void LoadSuperMetaData();

private:
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
	FAbxrAuthCallbacks CreateAuthCallbacks();
	void HandleAuthCompleted(const bool bSuccess) const;

	static void AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta);
	void Register(const FString& Key, const FString& Value, bool Overwrite);
	void SaveSuperMetaData() const;
	static bool IsReservedSuperMetaDataKey(const FString& Key);

	// Private helper function to merge super metadata and module info into metadata
	// Ensures data-specific metadata take precedence over super metadata and module info
	TMap<FString, FString> MergeSuperMetaData(TMap<FString, FString>& Meta);
	
	/// <summary>
	/// Set module metadata when no modules are provided in authentication.
	/// This method allows developers to track module information even when the LMS doesn't provide a module list.
	/// Only works when NOT using auth-provided module targets - returns safely if auth modules exist.
	/// Sets moduleName, moduleId, and moduleOrder in super metadata for automatic inclusion in all events.
	/// </summary>
	/// <param name="Module">The target name of the module</param>
	/// <param name="ModuleName">Optional user friendly name of the module</param>
	void SetModule(const FString& Module, const FString& ModuleName);
	void SetModule(const FString& Module) { return SetModule(Module, FString()); }
	
	// Formats a module name to be more human-readable by adding spaces between words.
	// Converts camelCase and PascalCase to space-separated format.
	// Example: "ModuleName" -> "Module Name", "myModule" -> "My Module"
	static FString FormatModuleNameForDisplay(const FString& ModuleName);
	
	FDelegateHandle AppWillEnterBackgroundHandle;
	
	void AdvanceToNextModule();
	
	int CurrentModuleIndex = 0;
	
	UPROPERTY(Transient)
	TObjectPtr<UXRDMService> XRDMService = nullptr;
	
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
	
	static const FString PollEventString;
	static const FString PollResponseString;
	static const FString PollQuestionString;
};