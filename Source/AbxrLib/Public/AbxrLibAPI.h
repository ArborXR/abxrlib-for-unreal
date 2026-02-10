#pragma once
#include "CoreMinimal.h"
#include "AbxrLibAPI_Internal.h"
#include "Types/AbxrLog.h"

namespace Abxr
{
	ABXRLIB_API inline void Authenticate()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return;
		}
		Subsystem->Authenticate();
	}
	
	ABXRLIB_API inline FAbxrAuthCompleted& OnAuthCompleted()
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
	ABXRLIB_API inline FAbxrModuleTarget& OnModuleTarget()
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
	ABXRLIB_API inline FAbxrAllModulesCompleted& OnAllModulesCompleted()
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
	
	ABXRLIB_API inline TArray<FAbxrModuleData> GetModuleList()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return TArray<FAbxrModuleData>();
		}
		return Subsystem->GetModuleList();
	}
	
	ABXRLIB_API inline bool StartModuleAtIndex(const int ModuleIndex)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return false;
		}
		return Subsystem->StartModuleAtIndex(ModuleIndex);
	}
	
	ABXRLIB_API inline void LogDebug(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogDebug() failed."));
			return;
		}
		Subsystem->LogDebug(Text, Meta);
	}
	ABXRLIB_API inline void LogDebug(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogDebug(Text, Meta);
	}
	
	ABXRLIB_API inline void LogInfo(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogInfo() failed."));
			return;
		}
		Subsystem->LogInfo(Text, Meta);
	}
	ABXRLIB_API inline void LogInfo(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogInfo(Text, Meta);
	}
	
	ABXRLIB_API inline void LogWarn(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogWarn() failed."));
			return;
		}
		Subsystem->LogWarn(Text, Meta);
	}
	ABXRLIB_API inline void LogWarn(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogWarn(Text, Meta);
	}
	
	ABXRLIB_API inline void LogError(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogError() failed."));
			return;
		}
		Subsystem->LogError(Text, Meta);
	}
	ABXRLIB_API inline void LogError(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogError(Text, Meta);
	}
	
	ABXRLIB_API inline void LogCritical(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogCritical() failed."));
			return;
		}
		Subsystem->LogCritical(Text, Meta);
	}
	ABXRLIB_API inline void LogCritical(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogCritical(Text, Meta);
	}
	
	ABXRLIB_API inline void Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Log() failed."));
			return;
		}
		Subsystem->Log(Text, Level, Meta);
	}
	ABXRLIB_API inline void Log(const FString& Text, const ELogLevel Level)
	{
		TMap<FString, FString> Meta;
		Log(Text, Level, Meta);
	}
	ABXRLIB_API inline void Log(const FString& Text)
	{
		TMap<FString, FString> Meta;
		Log(Text, ELogLevel::Info, Meta);
	}
	
	ABXRLIB_API inline void Event(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Meta);
	}
	ABXRLIB_API inline void Event(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Event(Name, Meta);
	}
	ABXRLIB_API inline void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Position, Meta);
	}
	ABXRLIB_API inline void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}
	
	ABXRLIB_API inline void Telemetry(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Telemetry() failed."));
			return;
		}
		Subsystem->Telemetry(Name, Meta);
	}
	ABXRLIB_API inline void Telemetry(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Telemetry(Name, Meta);
	}
	
	ABXRLIB_API inline void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentStart() failed."));
			return;
		}
		Subsystem->EventAssessmentStart(AssessmentName, Meta);
	}
	ABXRLIB_API inline void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}
	
	ABXRLIB_API inline void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentComplete() failed."));
			return;
		}
		Subsystem->EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	ABXRLIB_API inline void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	
	ABXRLIB_API inline void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveStart() failed."));
			return;
		}
		Subsystem->EventObjectiveStart(ObjectiveName, Meta);
	}
	ABXRLIB_API inline void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}
	
	ABXRLIB_API inline void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveComplete() failed."));
			return;
		}
		Subsystem->EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	ABXRLIB_API inline void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	
	ABXRLIB_API inline void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionStart() failed."));
			return;
		}
		Subsystem->EventInteractionStart(InteractionName, Meta);
	}
	ABXRLIB_API inline void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}
	
	ABXRLIB_API inline void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionComplete() failed."));
			return;
		}
		Subsystem->EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	ABXRLIB_API inline void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	
	ABXRLIB_API inline void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelStart() failed."));
			return;
		}
		Subsystem->EventLevelStart(LevelName, Meta);
	}
	ABXRLIB_API inline void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}
	
	ABXRLIB_API inline void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelComplete() failed."));
			return;
		}
		Subsystem->EventLevelComplete(LevelName, Score, Meta);
	}
	ABXRLIB_API inline void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}
	
	ABXRLIB_API inline void EventCritical(const FString& Label, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventCritical() failed."));
			return;
		}
		Subsystem->EventCritical(Label, Meta);
	}
	ABXRLIB_API inline void EventCritical(const FString& Label)
	{
		TMap<FString, FString> Meta;
		EventCritical(Label, Meta);
	}
	
	// Gets the UUID assigned to device by ArborXR
	ABXRLIB_API inline FString GetDeviceId()
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
	ABXRLIB_API inline FString GetDeviceSerial()
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
	ABXRLIB_API inline FString GetDeviceTitle()
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
	ABXRLIB_API inline TArray<FString> GetDeviceTags()
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
	ABXRLIB_API inline FString GetOrgId()
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
	ABXRLIB_API inline FString GetOrgTitle()
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
	ABXRLIB_API inline FString GetOrgSlug()
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
	ABXRLIB_API inline FString GetMacAddressFixed()
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
	ABXRLIB_API inline FString GetMacAddressRandom()
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
	ABXRLIB_API inline bool GetIsAuthenticated()
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
	ABXRLIB_API inline FString GetAccessToken()
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
	ABXRLIB_API inline FString GetRefreshToken()
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
	ABXRLIB_API inline FDateTime GetExpiresDateUtc()
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
	ABXRLIB_API inline FString GetFingerprint()
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
	ABXRLIB_API inline void StartNewSession()
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
	ABXRLIB_API inline TMap<FString, FString> GetUserData()
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
	ABXRLIB_API inline void Register(const FString& Key, const FString& Value)
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
	ABXRLIB_API inline void RegisterOnce(const FString& Key, const FString& Value)
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
	ABXRLIB_API inline void Unregister(const FString& Key)
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
	ABXRLIB_API inline void Reset()
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
	ABXRLIB_API inline TMap<FString, FString> GetSuperMetaData()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetSuperMetaData() failed."));
			return TMap<FString, FString>();
		}
		return Subsystem->GetSuperMetaData();
	}

	ABXRLIB_API inline void LoadSuperMetaData()
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
