#include "AbxrSubsystem.h"
#include "AbxrLibAPI_Internal.h"
#include "Services/Config/AbxrSettings.h"
#include "Services/Platform/XRDM/XRDMService.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UI/AbxrUISubsystem.h"

const FString UAbxrSubsystem::SuperMetaDataKey(TEXT("AbxrSuperMetaData"));

void UAbxrSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
	AbxrLib_SetActiveSubsystem(this);
	AuthService = MakeShared<FAbxrAuthService>();
	DataService = MakeShared<FAbxrDataService>(*AuthService);
	SuperMetaData = TMap<FString, FString>();
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, 
		&UAbxrSubsystem::OnPostLoadMapWithWorld
	);
#if PLATFORM_ANDROID
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		XRDMService->Initialize();
		UE_LOG(LogTemp, Log, TEXT("XRDM Service singleton retrieved and initialized"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get XRDM Service singleton"));
	}
#endif
	LoadSuperMetaData();
	if (GetDefault<UAbxrSettings>()->EnableAutoStartAuth)
	{
		if (GetDefault<UAbxrSettings>()->AuthenticationStartDelay > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(
					AuthenticationTimerHandle,
					this,
					&UAbxrSubsystem::Authenticate,
					GetDefault<UAbxrSettings>()->AuthenticationStartDelay,
					false // don't loop
				);
		}
		else
		{
			Authenticate();
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AbxrLib: Auto-start authentication is disabled. Call UAbxr::Authenticate() manually when ready."));
	}
	
	DataService->Start();
}

void UAbxrSubsystem::Deinitialize()
{
	if (AuthService)
	{
		AuthService->StopReAuthPolling();
		AuthService.Reset();
	}
	
	if (DataService)
	{
		DataService->Stop();
		DataService.Reset();
	}
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AuthenticationTimerHandle);
	}
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	
	AbxrLib_ClearActiveSubsystem(this);
	
	Super::Deinitialize();
	bInitialized = false;
}

void UAbxrSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!LoadedWorld) return;
    
	const FString NewLevelName = LoadedWorld->GetName();
	if (NewLevelName != CurrentLevel)
	{
		CurrentLevel = NewLevelName;
		if (GetDefault<UAbxrSettings>()->EnableSceneEvents)
		{
			TMap<FString, FString> Meta;
			Meta.Add(TEXT("Scene Name"), NewLevelName);
			Event(TEXT("Scene Changed"), Meta);
		}
	}
}

void UAbxrSubsystem::Log(const FString& Text, const ELogLevel Level, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("Scene Name"), CurrentLevel);
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

	DataService->AddLog(LevelText, Text, Meta);
}

void UAbxrSubsystem::Event(const FString& Name, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("Scene Name"), CurrentLevel);
	MergeSuperMetaData(Meta);
	DataService->AddEvent(Name, Meta);
}

/**
* Add event information
*
* @param Name      Name of the event
* @param Position  Adds position tracking of the object
* @param Meta      Any additional information
*/
void UAbxrSubsystem::Event(const FString& Name, const FVector& Position, TMap<FString, FString>& Meta)
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
void UAbxrSubsystem::Telemetry(const FString& Name, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("Scene Name"), CurrentLevel);
	MergeSuperMetaData(Meta);
    DataService->AddTelemetry(Name, Meta);
}

void UAbxrSubsystem::EventAssessmentStart(const FString& AssessmentName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	AssessmentStartTimes.Add(AssessmentName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(AssessmentName, Meta);
}

void UAbxrSubsystem::EventAssessmentComplete(const FString& AssessmentName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("assessment"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(AssessmentStartTimes, AssessmentName, Meta);
	Event(AssessmentName, Meta);
	DataService->Send();
}

void UAbxrSubsystem::EventObjectiveStart(const FString& ObjectiveName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	ObjectiveStartTimes.Add(ObjectiveName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(ObjectiveName, Meta);
}

void UAbxrSubsystem::EventObjectiveComplete(const FString& ObjectiveName, const int Score, EEventStatus Status, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("objective"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	Meta.Add(TEXT("status"), StaticEnum<EEventStatus>()->GetNameStringByValue(static_cast<int64>(Status)));
	AddDuration(ObjectiveStartTimes, ObjectiveName, Meta);
	Event(ObjectiveName, Meta);
}

void UAbxrSubsystem::EventInteractionStart(const FString& InteractionName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("started"));
	InteractionStartTimes.Add(InteractionName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(InteractionName, Meta);
}

void UAbxrSubsystem::EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("type"), TEXT("interaction"));
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("interaction"), StaticEnum<EInteractionType>()->GetNameStringByValue(static_cast<int64>(InteractionType)));
	if (!Response.IsEmpty()) Meta.Add(TEXT("response"), Response);
	AddDuration(InteractionStartTimes, InteractionName, Meta);
	Event(InteractionName, Meta);
}

void UAbxrSubsystem::EventLevelStart(const FString& LevelName, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("started"));
	Meta.Add(TEXT("id"), LevelName);
	LevelStartTimes.Add(LevelName, FDateTime::UtcNow().ToUnixTimestamp());
	Event(TEXT("level_start"), Meta);
}

void UAbxrSubsystem::EventLevelComplete(const FString& LevelName, const int Score, TMap<FString, FString>& Meta)
{
	Meta.Add(TEXT("verb"), TEXT("completed"));
	Meta.Add(TEXT("id"), LevelName);
	Meta.Add(TEXT("score"), FString::FromInt(Score));
	AddDuration(LevelStartTimes, LevelName, Meta);
	Event(LevelName, Meta);
}

void UAbxrSubsystem::EventCritical(const FString& Label, TMap<FString, FString>& Meta)
{
	const FString TaggedName = TEXT("CRITICAL_ABXR_") + Label;
	Event(TaggedName, Meta);
}

void UAbxrSubsystem::AddDuration(TMap<FString, int64>& StartTimes, const FString& Name, TMap<FString, FString>& Meta)
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

void UAbxrSubsystem::PresentKeyboard(const FString& PromptText, const FString& KeyboardType, const FString& EmailDomain) const
{
	if (UAbxrUISubsystem* Subsys = GetGameInstance()->GetSubsystem<UAbxrUISubsystem>())
	{
		Subsys->ShowKeyboardUI(FText::FromString(PromptText));
	}
}

FString UAbxrSubsystem::GetDeviceId()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceId();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetDeviceSerial()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceSerial();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetDeviceTitle()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceTitle();
	}

	return TEXT("");
}

TArray<FString> UAbxrSubsystem::GetDeviceTags()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetDeviceTags();
	}

	return TArray<FString>();
}

FString UAbxrSubsystem::GetOrgId()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgId();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetOrgTitle()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgTitle();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetOrgSlug()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetOrgSlug();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetMacAddressFixed()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetMacAddressFixed();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetMacAddressRandom()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetMacAddressRandom();
	}

	return TEXT("");
}

bool UAbxrSubsystem::GetIsAuthenticated()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetIsAuthenticated();
	}

	return false;
}

FString UAbxrSubsystem::GetAccessToken()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetAccessToken();
	}

	return TEXT("");
}

FString UAbxrSubsystem::GetRefreshToken()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetRefreshToken();
	}

	return TEXT("");
}

FDateTime UAbxrSubsystem::GetExpiresDateUtc()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetExpiresDateUtc();
	}

	return FDateTime::MinValue();
}

FString UAbxrSubsystem::GetFingerprint()
{
	if (const UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		return XRDMService->GetFingerprint();
	}

	return TEXT("");
}

void UAbxrSubsystem::StartNewSession() const
{
	AuthService->SetSessionId(FGuid::NewGuid().ToString());
	Authenticate();
}

TMap<FString, FString> UAbxrSubsystem::GetUserData() const
{
	return AuthService->GetAuthResponse().UserData;
}

void UAbxrSubsystem::Register(const FString& Key, const FString& Value, const bool Overwrite)
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

void UAbxrSubsystem::Register(const FString& Key, const FString& Value)
{
	Register(Key, Value, true);
}

void UAbxrSubsystem::RegisterOnce(const FString& Key, const FString& Value)
{
	Register(Key, Value, false);
}

void UAbxrSubsystem::Unregister(const FString& Key)
{
	SuperMetaData.Remove(Key);
	SaveSuperMetaData();
}

void UAbxrSubsystem::Reset()
{
	SuperMetaData.Empty();
	SaveSuperMetaData();
}

TMap<FString, FString> UAbxrSubsystem::GetSuperMetaData()
{
	return SuperMetaData;
}

void UAbxrSubsystem::LoadSuperMetaData()
{
	if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SuperMetaDataKey, 0))
	{
		if (const USuperMetaSave* SaveObject = Cast<USuperMetaSave>(Loaded))
		{
			SuperMetaData = SaveObject->SuperMetaData;
		}
	}
}

void UAbxrSubsystem::SaveSuperMetaData() const
{
	USuperMetaSave* SaveObject = Cast<USuperMetaSave>(UGameplayStatics::CreateSaveGameObject(USuperMetaSave::StaticClass()));
	if (!SaveObject) return;

	SaveObject->SuperMetaData = SuperMetaData;
	UGameplayStatics::SaveGameToSlot(SaveObject, SuperMetaDataKey, 0);
}

TMap<FString, FString> UAbxrSubsystem::MergeSuperMetaData(TMap<FString, FString>& Meta)
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

bool UAbxrSubsystem::IsReservedSuperMetaDataKey(const FString& Key)
{
	return Key == TEXT("module") || Key == TEXT("moduleName") || Key == TEXT("moduleId") || Key == TEXT("moduleOrder");
}