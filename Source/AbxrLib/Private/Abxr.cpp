#include "Abxr.h"
#include "AbxrWorldSubsystem.h"
#include "Authentication.h"
#include "DataBatcher.h"
#include "LevelTracker.h"
#include "XRDMService.h"
#include "Kismet/GameplayStatics.h"

TMap<FString, int64> UAbxr::AssessmentStartTimes;
TMap<FString, int64> UAbxr::ObjectiveStartTimes;
TMap<FString, int64> UAbxr::InteractionStartTimes;
TMap<FString, int64> UAbxr::LevelStartTimes;
TWeakObjectPtr<UWorld> UAbxr::GWorldWeak;
TMap<FString, FString> UAbxr::SuperMetaData = TMap<FString, FString>();
const FString UAbxr::SuperMetaDataKey = TEXT("AbxrSuperMetaData");
FString LevelTracker::CurrentLevel = TEXT("N/A");


void UAbxr::LogDebug(const FString& Text, const TMap<FString, FString>& Meta)
{
	Log(Text, ELogLevel::Debug, Meta);
}

void UAbxr::LogInfo(const FString& Text, const TMap<FString, FString>& Meta)
{
	Log(Text, ELogLevel::Info, Meta);
}

void UAbxr::LogWarn(const FString& Text, const TMap<FString, FString>& Meta)
{
	Log(Text, ELogLevel::Warn, Meta);
}

void UAbxr::LogError(const FString& Text, const TMap<FString, FString>& Meta)
{
	Log(Text, ELogLevel::Error, Meta);
}

void UAbxr::LogCritical(const FString& Text, const TMap<FString, FString>& Meta)
{
	Log(Text, ELogLevel::Critical, Meta);
}

void UAbxr::Log(const FString& Text, const ELogLevel Level, TMap<FString, FString> Meta)
{
	Meta.Add(TEXT("Scene Name"), LevelTracker::GetCurrentLevel());
	MergeSuperMetaData(Meta);
	FString LevelText;
    switch (Level)
    {
        case ELogLevel::Debug:
    		LevelText = "debug";
            break;
        case ELogLevel::Info:
    		LevelText = "info";
            break;
        case ELogLevel::Warn:
    		LevelText = "warn";
            break;
        case ELogLevel::Error:
    		LevelText = "error";
            break;
        case ELogLevel::Critical:
    		LevelText = "critical";
            break;
        default:
    		LevelText = "info";
            break;
    }

	DataBatcher::AddLog(LevelText, Text, Meta);
}

void UAbxr::Event(const FString& Name, TMap<FString, FString> Meta)
{
	Meta.Add(TEXT("Scene Name"), LevelTracker::GetCurrentLevel());
	MergeSuperMetaData(Meta);
	DataBatcher::AddEvent(Name, Meta);
}

/**
* Add event information
*
* @param Name      Name of the event
* @param Position  Adds position tracking of the object
* @param Meta      Any additional information
*/
void UAbxr::Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
{
    Meta.Add(TEXT("position_x"), FString::SanitizeFloat(Position.X));
    Meta.Add(TEXT("position_y"), FString::SanitizeFloat(Position.Y));
    Meta.Add(TEXT("position_z"), FString::SanitizeFloat(Position.Z));
    Event(Name, Meta);
}

/**
* Add telemetry information
*
* @param Name      Name of the telemetry
* @param Meta      Any additional information
*/
void UAbxr::Telemetry(const FString& Name, TMap<FString, FString> Meta)
{
	Meta.Add(TEXT("Scene Name"), LevelTracker::GetCurrentLevel());
	MergeSuperMetaData(Meta);
    DataBatcher::AddTelemetry(Name, Meta);
}

void UAbxr::EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	AssessmentStartTimes.Add(AssessmentName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(AssessmentName, Meta);
}

void UAbxr::EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(AssessmentStartTimes, AssessmentName, Meta);
	Event(AssessmentName, Meta);
	DataBatcher::Send();
}

void UAbxr::EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	ObjectiveStartTimes.Add(ObjectiveName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(ObjectiveName, Meta);
}

void UAbxr::EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(ObjectiveStartTimes, ObjectiveName, Meta);
	Event(ObjectiveName, Meta);
}

void UAbxr::EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	InteractionStartTimes.Add(InteractionName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(InteractionName, Meta);
}

void UAbxr::EventInteractionComplete(const FString& InteractionName, EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("interaction"), StaticEnum<EInteractionType>()->GetNameStringByValue(static_cast<int64>(InteractionType)));
	if (!Response.IsEmpty()) Meta.Add(TEXT("response"), Response);
	AddDuration(InteractionStartTimes, InteractionName, Meta);
	Event(InteractionName, Meta);
}

void UAbxr::EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("started"));
	Meta.Add(TEXT("id"), LevelName);
	LevelStartTimes.Add(LevelName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(TEXT("level_start"), Meta);
}

void UAbxr::EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("id"), LevelName);
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	AddDuration(LevelStartTimes, LevelName, Meta);
	Event(LevelName, Meta);
}

void UAbxr::EventCritical(const FString& Label, const TMap<FString, FString>& Meta)
{
	const FString taggedName = TEXT("CRITICAL_ABXR_") + Label;
	Event(taggedName, Meta);
}

void UAbxr::AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta)
{
	if (StartTimes.Contains(Name))
	{
		const int64 Duration = FDateTime::UtcNow().ToUnixTimestamp() - StartTimes[Name];
		Meta.Add(TEXT("duration"), FString::Printf(TEXT("%lld"), Duration));
		StartTimes.Remove(Name);
	}
	else
	{
		Meta.Add(TEXT("duration"), TEXT("0"));
	}
}

void UAbxr::PresentKeyboard(const FString& PromptText, const FString& KeyboardType, const FString& EmailDomain)
{
	TWeakObjectPtr<UWorld> Snap = GWorldWeak;
	if (UAbxrWorldSubsystem* Subsys = Snap.Get()->GetSubsystem<UAbxrWorldSubsystem>())
	{
		Subsys->ShowKeyboardUI(400.f, 0.f);
	}
}

FString UAbxr::GetDeviceId()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceId();
	}

	return TEXT("");
}

FString UAbxr::GetDeviceSerial()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceSerial();
	}

	return TEXT("");
}

FString UAbxr::GetDeviceTitle()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceTitle();
	}

	return TEXT("");
}

TArray<FString> UAbxr::GetDeviceTags()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceTags();
	}

	return TArray<FString>();
}

FString UAbxr::GetOrgId()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgId();
	}

	return TEXT("");
}

FString UAbxr::GetOrgTitle()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgTitle();
	}

	return TEXT("");
}

FString UAbxr::GetOrgSlug()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgSlug();
	}

	return TEXT("");
}

FString UAbxr::GetMacAddressFixed()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetMacAddressFixed();
	}

	return TEXT("");
}

FString UAbxr::GetMacAddressRandom()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetMacAddressRandom();
	}

	return TEXT("");
}

bool UAbxr::GetIsAuthenticated()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetIsAuthenticated();
	}

	return false;
}

FString UAbxr::GetAccessToken()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetAccessToken();
	}

	return TEXT("");
}

FString UAbxr::GetRefreshToken()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetRefreshToken();
	}

	return TEXT("");
}

FDateTime UAbxr::GetExpiresDateUtc()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetExpiresDateUtc();
	}

	return FDateTime::MinValue();
}

FString UAbxr::GetFingerprint()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetFingerprint();
	}

	return TEXT("");
}

void UAbxr::StartNewSession()
{
	Authentication::SetSessionId(FGuid::NewGuid().ToString());
	Authenticate();
}

void UAbxr::Register(const FString& Key, const FString& Value, const bool Overwrite)
{
	if (IsReservedSuperMetaDataKey(Key))
	{
		const FString ErrorMessage = TEXT("AbxrLib: Cannot register super metadata with reserved key '") + Key +
			TEXT("'. Reserved keys are: module, moduleName, moduleId, moduleOrder");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMessage);
		TMap<FString, FString> Meta;
		Meta.Add(TEXT("attempted_key"), Key);
		Meta.Add(TEXT("attempted_value"), Value);
		Meta.Add(TEXT("error_type"), TEXT("reserved_super_metadata_key"));
		return;
	}

	if (Overwrite || !SuperMetaData.Contains(Key))
	{
		SuperMetaData.Add(Key, Value);
		SaveSuperMetaData();
	}
}

void UAbxr::Register(const FString& Key, const FString& Value)
{
	Register(Key, Value, true);
}

void UAbxr::RegisterOnce(const FString& Key, const FString& Value)
{
	Register(Key, Value, false);
}

void UAbxr::Unregister(const FString& Key)
{
	SuperMetaData.Remove(Key);
	SaveSuperMetaData();
}

void UAbxr::Reset()
{
	SuperMetaData.Empty();
	SaveSuperMetaData();
}

TMap<FString, FString> UAbxr::GetSuperMetaData()
{
	return SuperMetaData;
}

void UAbxr::LoadSuperMetaData()
{
	if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SuperMetaDataKey, 0))
	{
		if (const USuperMetaSave* SaveObject = Cast<USuperMetaSave>(Loaded))
		{
			SuperMetaData = SaveObject->SuperMetaData;
		}
	}
}

void UAbxr::SaveSuperMetaData()
{
	USuperMetaSave* SaveObject = Cast<USuperMetaSave>(UGameplayStatics::CreateSaveGameObject(USuperMetaSave::StaticClass()));
	if (!SaveObject) return;

	SaveObject->SuperMetaData = SuperMetaData;
	UGameplayStatics::SaveGameToSlot(SaveObject, SuperMetaDataKey, 0);
}

TMap<FString, FString> UAbxr::MergeSuperMetaData(TMap<FString, FString>& Meta)
{
	// Add super metadata to metadata (includes manually-set moduleName/moduleId/moduleOrder when no LMS modules)
	// Auth-provided module metadata takes precedence, so manually-set values only appear when no LMS modules exist
	for (const TPair<FString, FString>& SuperMetaDataKeyValue : SuperMetaData)
	{
		// super metadata don't overwrite data-specific metadata or auth-provided module info
		if (!Meta.Contains(SuperMetaDataKeyValue.Key))
		{
			Meta.Add(SuperMetaDataKeyValue.Key, SuperMetaDataKeyValue.Value);
		}
	}
	
	return Meta;
}

bool UAbxr::IsReservedSuperMetaDataKey(const FString& Key)
{
	return Key == TEXT("module") || Key == TEXT("moduleName") || Key == TEXT("moduleId") || Key == TEXT("moduleOrder");
}
