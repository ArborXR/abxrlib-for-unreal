#include "AbxrLibAPI.h"
#include "AbxrLibAPI_Internal.h"
#include "Types/AbxrLog.h"

namespace Abxr
{
	void Authenticate()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return;
		}
		Subsystem->Authenticate();
	}
	
	FAbxrAuthCompleted& OnAuthCompleted()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. OnAuthCompleted() binding will be ignored."));
			static FAbxrAuthCompleted Dummy;
			return Dummy;
		}
		return Subsystem->OnAuthCompleted;
	}
	
	// Fired when a new module target becomes active (LMS-driven sequencing)
	FAbxrModuleTarget& OnModuleTarget()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. OnModuleTarget() binding will be ignored."));
			static FAbxrModuleTarget Dummy;
			return Dummy;
		}
		return Subsystem->OnModuleTarget;
	}

	// Fired when module sequencing has completed
	FAbxrAllModulesCompleted& OnAllModulesCompleted()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. OnAllModulesCompleted() binding will be ignored."));
			static FAbxrAllModulesCompleted Dummy;
			return Dummy;
		}
		return Subsystem->OnAllModulesCompleted;
	}
	
	TArray<FAbxrModuleData> GetModuleList()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return TArray<FAbxrModuleData>();
		}
		return Subsystem->GetModuleList();
	}
	
	bool StartModuleAtIndex(const int ModuleIndex)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return false;
		}
		return Subsystem->StartModuleAtIndex(ModuleIndex);
	}
	
	void LogDebug(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogDebug() failed."));
			return;
		}
		Subsystem->LogDebug(Text, Meta);
	}
	void LogDebug(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogDebug(Text, Meta);
	}
	
	void LogInfo(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogInfo() failed."));
			return;
		}
		Subsystem->LogInfo(Text, Meta);
	}
	void LogInfo(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogInfo(Text, Meta);
	}
	
	void LogWarn(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogWarn() failed."));
			return;
		}
		Subsystem->LogWarn(Text, Meta);
	}
	void LogWarn(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogWarn(Text, Meta);
	}
	
	void LogError(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogError() failed."));
			return;
		}
		Subsystem->LogError(Text, Meta);
	}
	void LogError(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogError(Text, Meta);
	}
	
	void LogCritical(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogCritical() failed."));
			return;
		}
		Subsystem->LogCritical(Text, Meta);
	}
	void LogCritical(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogCritical(Text, Meta);
	}
	
	void Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Log() failed."));
			return;
		}
		Subsystem->Log(Text, Level, Meta);
	}
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
	
	void Event(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Meta);
	}
	void Event(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Event(Name, Meta);
	}
	void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Position, Meta);
	}
	void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}
	
	void Telemetry(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Telemetry() failed."));
			return;
		}
		Subsystem->Telemetry(Name, Meta);
	}
	void Telemetry(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Telemetry(Name, Meta);
	}
	
	void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentStart() failed."));
			return;
		}
		Subsystem->EventAssessmentStart(AssessmentName, Meta);
	}
	void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}
	
	void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentComplete() failed."));
			return;
		}
		Subsystem->EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	
	void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveStart() failed."));
			return;
		}
		Subsystem->EventObjectiveStart(ObjectiveName, Meta);
	}
	void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}
	
	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveComplete() failed."));
			return;
		}
		Subsystem->EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	
	void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionStart() failed."));
			return;
		}
		Subsystem->EventInteractionStart(InteractionName, Meta);
	}
	void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}
	
	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionComplete() failed."));
			return;
		}
		Subsystem->EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	
	void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelStart() failed."));
			return;
		}
		Subsystem->EventLevelStart(LevelName, Meta);
	}
	void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}
	
	void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelComplete() failed."));
			return;
		}
		Subsystem->EventLevelComplete(LevelName, Score, Meta);
	}
	void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}
	
	void EventCritical(const FString& Label, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventCritical() failed."));
			return;
		}
		Subsystem->EventCritical(Label, Meta);
	}
	void EventCritical(const FString& Label)
	{
		TMap<FString, FString> Meta;
		EventCritical(Label, Meta);
	}
	
	// Gets the UUID assigned to device by ArborXR
	FString GetDeviceId()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceId() failed."));
			return FString();
		}
		return Subsystem->GetDeviceId();
	}

	// Gets the serial number assigned to device by OEM
	FString GetDeviceSerial()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceSerial() failed."));
			return FString();
		}
		return Subsystem->GetDeviceSerial();
	}

	// Gets the title given to device by admin through the ArborXR Web Portal
	FString GetDeviceTitle()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceTitle() failed."));
			return FString();
		}
		return Subsystem->GetDeviceTitle();
	}

	// Gets the tags added to device by admin through the ArborXR Web Portal
	TArray<FString> GetDeviceTags()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceTags() failed."));
			return TArray<FString>();
		}
		return Subsystem->GetDeviceTags();
	}

	// Gets the UUID of the organization where the device is assigned. Organizations are created in the ArborXR Web Portal
	FString GetOrgId()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgId() failed."));
			return FString();
		}
		return Subsystem->GetOrgId();
	}

	// Gets the name assigned to organization by admin through the ArborXR Web Portal
	FString GetOrgTitle()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgTitle() failed."));
			return FString();
		}
		return Subsystem->GetOrgTitle();
	}

	// Gets the identifier generated by ArborXR when admin assigns title to organization
	FString GetOrgSlug()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgSlug() failed."));
			return FString();
		}
		return Subsystem->GetOrgSlug();
	}

	// Gets the physical MAC address assigned to device by OEM
	FString GetMacAddressFixed()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetMacAddressFixed() failed."));
			return FString();
		}
		return Subsystem->GetMacAddressFixed();
	}

	// Gets the randomized MAC address for the current WiFi connection
	FString GetMacAddressRandom()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetMacAddressRandom() failed."));
			return FString();
		}
		return Subsystem->GetMacAddressRandom();
	}

	// Gets whether the device is SSO authenticated
	bool GetIsAuthenticated()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetIsAuthenticated() failed."));
			return false;
		}
		return Subsystem->GetIsAuthenticated();
	}

	// Gets SSO access token
	FString GetAccessToken()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetAccessToken() failed."));
			return FString();
		}
		return Subsystem->GetAccessToken();
	}

	// Gets SSO refresh token
	FString GetRefreshToken()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetRefreshToken() failed."));
			return FString();
		}
		return Subsystem->GetRefreshToken();
	}

	// Gets SSO token remaining lifetime
	FDateTime GetExpiresDateUtc()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetExpiresDateUtc() failed."));
			return FDateTime();
		}
		return Subsystem->GetExpiresDateUtc();
	}

	// Gets the device fingerprint
	FString GetFingerprint()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetFingerprint() failed."));
			return FString();
		}
		return Subsystem->GetFingerprint();
	}
	
	// Start a new session with a fresh session identifier
	// Generates a new session ID and performs fresh authentication
	// Useful for starting new training experiences or resetting user context
	void StartNewSession()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. StartNewSession() failed."));
			return;
		}
		return Subsystem->StartNewSession();
	}

	// Get the learner/user data from the most recent authentication completion
	// This is the userData object from the authentication response, containing user preferences and information
	// Returns an empty map if no authentication has completed yet
	TMap<FString, FString> GetUserData()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetUserData() failed."));
			return TMap<FString, FString>();
		}
		return Subsystem->GetUserData();
	}

	// Register a super metadata that will be automatically included in all events
	// super metadata persist across app sessions and are stored locally
	void Register(const FString& Key, const FString& Value)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Register() failed."));
			return;
		}
		Subsystem->Register(Key, Value);
	}

	// Register a super metadata only if it doesn't already exist
	// Will not overwrite existing super metadata with the same key
	void RegisterOnce(const FString& Key, const FString& Value)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. RegisterOnce() failed."));
			return;
		}
		Subsystem->RegisterOnce(Key, Value);
	}
	
	// Remove a super metadata entry
	void Unregister(const FString& Key)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Unregister() failed."));
			return;
		}
		Subsystem->Unregister(Key);
	}

	// Clear all super metadata
	// Clears all super metadata from persistent storage
	void Reset()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Reset() failed."));
			return;
		}
		Subsystem->Reset();
	}

	// Get a copy of all current super metadata
	TMap<FString, FString> GetSuperMetaData()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetSuperMetaData() failed."));
			return TMap<FString, FString>();
		}
		return Subsystem->GetSuperMetaData();
	}

	void LoadSuperMetaData()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LoadSuperMetaData() failed."));
			return;
		}
		Subsystem->LoadSuperMetaData();
	}
}
