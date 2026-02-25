#include "AbxrLibAPI.h"
#include "Types/AbxrLog.h"
#include "AbxrLibAPI_Internal.h"

namespace Abxr
{
	ABXRLIB_API void Authenticate()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return;
		}
		Subsystem->Authenticate();
	}
	
	ABXRLIB_API FAbxrAuthCompleted& OnAuthCompleted()
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
	
	ABXRLIB_API FAbxrModuleTarget& OnModuleTarget()
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
	
	ABXRLIB_API FAbxrAllModulesCompleted& OnAllModulesCompleted()
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
	
	ABXRLIB_API TArray<FAbxrModuleData> GetModuleList()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return TArray<FAbxrModuleData>();
		}
		return Subsystem->GetModuleList();
	}
	
	ABXRLIB_API bool StartModuleAtIndex(const int ModuleIndex)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Authenticate() failed."));
			return false;
		}
		return Subsystem->StartModuleAtIndex(ModuleIndex);
	}
	
	ABXRLIB_API void LogDebug(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogDebug() failed."));
			return;
		}
		Subsystem->LogDebug(Text, Meta);
	}
	ABXRLIB_API void LogDebug(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogDebug(Text, Meta);
	}
	
	ABXRLIB_API void LogInfo(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogInfo() failed."));
			return;
		}
		Subsystem->LogInfo(Text, Meta);
	}
	ABXRLIB_API void LogInfo(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogInfo(Text, Meta);
	}
	
	ABXRLIB_API void LogWarn(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogWarn() failed."));
			return;
		}
		Subsystem->LogWarn(Text, Meta);
	}
	ABXRLIB_API void LogWarn(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogWarn(Text, Meta);
	}
	
	ABXRLIB_API void LogError(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogError() failed."));
			return;
		}
		Subsystem->LogError(Text, Meta);
	}
	ABXRLIB_API void LogError(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogError(Text, Meta);
	}
	
	ABXRLIB_API void LogCritical(const FString& Text, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. LogCritical() failed."));
			return;
		}
		Subsystem->LogCritical(Text, Meta);
	}
	ABXRLIB_API void LogCritical(const FString& Text)
	{
		TMap<FString, FString> Meta;
		LogCritical(Text, Meta);
	}
	
	ABXRLIB_API void Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Log() failed."));
			return;
		}
		Subsystem->Log(Text, Level, Meta);
	}
	ABXRLIB_API void Log(const FString& Text, const ELogLevel Level)
	{
		TMap<FString, FString> Meta;
		Log(Text, Level, Meta);
	}
	ABXRLIB_API void Log(const FString& Text)
	{
		TMap<FString, FString> Meta;
		Log(Text, ELogLevel::Info, Meta);
	}
	
	ABXRLIB_API void Event(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Meta);
	}
	ABXRLIB_API void Event(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Event(Name, Meta);
	}
	ABXRLIB_API void Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Event() failed."));
			return;
		}
		Subsystem->Event(Name, Position, Meta);
	}
	ABXRLIB_API void Event(const FString& Name, const FVector& Position)
	{
		TMap<FString, FString> Meta;
		Event(Name, Position, Meta);
	}
	
	ABXRLIB_API void Telemetry(const FString& Name, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Telemetry() failed."));
			return;
		}
		Subsystem->Telemetry(Name, Meta);
	}
	ABXRLIB_API void Telemetry(const FString& Name)
	{
		TMap<FString, FString> Meta;
		Telemetry(Name, Meta);
	}
	
	ABXRLIB_API void EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentStart() failed."));
			return;
		}
		Subsystem->EventAssessmentStart(AssessmentName, Meta);
	}
	ABXRLIB_API void EventAssessmentStart(const FString& AssessmentName)
	{
		TMap<FString, FString> Meta;
		EventAssessmentStart(AssessmentName, Meta);
	}
	
	ABXRLIB_API void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventAssessmentComplete() failed."));
			return;
		}
		Subsystem->EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	ABXRLIB_API void EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventAssessmentComplete(AssessmentName, Score, Status, Meta);
	}
	
	ABXRLIB_API void EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveStart() failed."));
			return;
		}
		Subsystem->EventObjectiveStart(ObjectiveName, Meta);
	}
	ABXRLIB_API void EventObjectiveStart(const FString& ObjectiveName)
	{
		TMap<FString, FString> Meta;
		EventObjectiveStart(ObjectiveName, Meta);
	}
	
	ABXRLIB_API void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventObjectiveComplete() failed."));
			return;
		}
		Subsystem->EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	ABXRLIB_API void EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status)
	{
		TMap<FString, FString> Meta;
		EventObjectiveComplete(ObjectiveName, Score, Status, Meta);
	}
	
	ABXRLIB_API void EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionStart() failed."));
			return;
		}
		Subsystem->EventInteractionStart(InteractionName, Meta);
	}
	ABXRLIB_API void EventInteractionStart(const FString& InteractionName)
	{
		TMap<FString, FString> Meta;
		EventInteractionStart(InteractionName, Meta);
	}
	
	ABXRLIB_API void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventInteractionComplete() failed."));
			return;
		}
		Subsystem->EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	ABXRLIB_API void EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response)
	{
		TMap<FString, FString> Meta;
		EventInteractionComplete(InteractionName, InteractionType, Response, Meta);
	}
	
	ABXRLIB_API void EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelStart() failed."));
			return;
		}
		Subsystem->EventLevelStart(LevelName, Meta);
	}
	ABXRLIB_API void EventLevelStart(const FString& LevelName)
	{
		TMap<FString, FString> Meta;
		EventLevelStart(LevelName, Meta);
	}
	
	ABXRLIB_API void EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventLevelComplete() failed."));
			return;
		}
		Subsystem->EventLevelComplete(LevelName, Score, Meta);
	}
	ABXRLIB_API void EventLevelComplete(const FString& LevelName, const int Score)
	{
		TMap<FString, FString> Meta;
		EventLevelComplete(LevelName, Score, Meta);
	}
	
	ABXRLIB_API void EventCritical(const FString& Label, TMap<FString, FString>& Meta)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. EventCritical() failed."));
			return;
		}
		Subsystem->EventCritical(Label, Meta);
	}
	ABXRLIB_API void EventCritical(const FString& Label)
	{
		TMap<FString, FString> Meta;
		EventCritical(Label, Meta);
	}
	
	ABXRLIB_API FString GetDeviceId()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceId() failed."));
			return FString();
		}
		return Subsystem->GetDeviceId();
	}
	
	ABXRLIB_API FString GetDeviceSerial()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceSerial() failed."));
			return FString();
		}
		return Subsystem->GetDeviceSerial();
	}
	
	ABXRLIB_API FString GetDeviceTitle()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceTitle() failed."));
			return FString();
		}
		return Subsystem->GetDeviceTitle();
	}
	
	ABXRLIB_API TArray<FString> GetDeviceTags()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetDeviceTags() failed."));
			return TArray<FString>();
		}
		return Subsystem->GetDeviceTags();
	}
	
	ABXRLIB_API FString GetOrgId()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgId() failed."));
			return FString();
		}
		return Subsystem->GetOrgId();
	}
	
	ABXRLIB_API FString GetOrgTitle()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgTitle() failed."));
			return FString();
		}
		return Subsystem->GetOrgTitle();
	}
	
	ABXRLIB_API FString GetOrgSlug()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetOrgSlug() failed."));
			return FString();
		}
		return Subsystem->GetOrgSlug();
	}
	
	ABXRLIB_API FString GetMacAddressFixed()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetMacAddressFixed() failed."));
			return FString();
		}
		return Subsystem->GetMacAddressFixed();
	}
	
	ABXRLIB_API FString GetMacAddressRandom()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetMacAddressRandom() failed."));
			return FString();
		}
		return Subsystem->GetMacAddressRandom();
	}
	
	ABXRLIB_API bool GetIsAuthenticated()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetIsAuthenticated() failed."));
			return false;
		}
		return Subsystem->GetIsAuthenticated();
	}
	
	ABXRLIB_API FString GetAccessToken()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetAccessToken() failed."));
			return FString();
		}
		return Subsystem->GetAccessToken();
	}
	
	ABXRLIB_API FString GetRefreshToken()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetRefreshToken() failed."));
			return FString();
		}
		return Subsystem->GetRefreshToken();
	}
	
	ABXRLIB_API FDateTime GetExpiresDateUtc()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetExpiresDateUtc() failed."));
			return FDateTime();
		}
		return Subsystem->GetExpiresDateUtc();
	}
	
	ABXRLIB_API FString GetFingerprint()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetFingerprint() failed."));
			return FString();
		}
		return Subsystem->GetFingerprint();
	}
	
	ABXRLIB_API void StartNewSession()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. StartNewSession() failed."));
			return;
		}
		return Subsystem->StartNewSession();
	}
	
	ABXRLIB_API TMap<FString, FString> GetUserData()
	{
		const UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetUserData() failed."));
			return TMap<FString, FString>();
		}
		return Subsystem->GetUserData();
	}
	
	ABXRLIB_API void Register(const FString& Key, const FString& Value)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Register() failed."));
			return;
		}
		Subsystem->Register(Key, Value);
	}
	
	ABXRLIB_API void RegisterOnce(const FString& Key, const FString& Value)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. RegisterOnce() failed."));
			return;
		}
		Subsystem->RegisterOnce(Key, Value);
	}
	
	ABXRLIB_API void Unregister(const FString& Key)
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Unregister() failed."));
			return;
		}
		Subsystem->Unregister(Key);
	}
	
	ABXRLIB_API void Reset()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. Reset() failed."));
			return;
		}
		Subsystem->Reset();
	}
	
	ABXRLIB_API TMap<FString, FString> GetSuperMetaData()
	{
		UAbxrSubsystem* Subsystem = AbxrLib_GetActiveSubsystem();
		if (Subsystem == nullptr)
		{
			UE_LOG(LogAbxrLib, Warning, TEXT("Not initialized yet. GetSuperMetaData() failed."));
			return TMap<FString, FString>();
		}
		return Subsystem->GetSuperMetaData();
	}

	ABXRLIB_API void LoadSuperMetaData()
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
