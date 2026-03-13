#include "AbxrSubsystem.h"
#include "AbxrLibAPI_Internal.h"
#include "TimerManager.h"
#include "Services/Config/AbxrSettings.h"
#include "Services/Platform/XRDM/XRDMService.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UI/AbxrUISubsystem.h"
#include "Async/Async.h"
#include "Types/AbxrLog.h"
#include "Util/AbxrUtil.h"

const FString UAbxrSubsystem::SuperMetaDataKey(TEXT("AbxrSuperMetaData"));

void UAbxrSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
	AbxrLib_SetActiveSubsystem(this);
#if PLATFORM_ANDROID
	if (FAbxrUtil::IsPackageInstalled(TEXT("app.xrdm.client")))
	{
		XRDMService = NewObject<UXRDMService>(this);
		if (XRDMService)
		{
			XRDMService->Initialize();
			UE_LOG(LogAbxrLib, Log, TEXT("XRDM Service created and initialized"));
		}
		else
		{
			UE_LOG(LogAbxrLib, Error, TEXT("Failed to create XRDM Service"));
		}
	}
	else
	{
		UE_LOG(LogAbxrLib, Warning, TEXT("XRDM Service not installed"));
	}
#endif
	AuthService = MakeShared<FAbxrAuthService>(CreateAuthCallbacks(), XRDMService);
	DataService = MakeShared<FAbxrDataService>(*AuthService);
	SuperMetaData = TMap<FString, FString>();
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, 
		&UAbxrSubsystem::OnPostLoadMapWithWorld
	);
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
		UE_LOG(LogAbxrLib, Log, TEXT("Auto-start authentication is disabled. Call UAbxr::Authenticate() manually when ready."));
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
	
	if (XRDMService)
	{
		XRDMService->Shutdown();
		XRDMService = nullptr;
	}
	
	AbxrLib_ClearActiveSubsystem(this);
	
	Super::Deinitialize();
	bInitialized = false;
}

FAbxrAuthCallbacks UAbxrSubsystem::CreateAuthCallbacks()
{
	FAbxrAuthCallbacks Callbacks;
	Callbacks.OnInputRequested = [WeakThis = TWeakObjectPtr(this)](const FAbxrKeyboardRequest& Request)
	{
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Request]
		{
			if (!WeakThis.IsValid()) return;
			UAbxrSubsystem* Self = WeakThis.Get();
			Self->OnInputRequested(Request);
			if (!Self->bIsPopupVisible)
			{
				Self->OnPopupShown.Broadcast();
				Self->bIsPopupVisible = true;
			}
		});
	};
	Callbacks.OnSucceeded = [WeakThis = TWeakObjectPtr(this)]
	{
		AsyncTask(ENamedThreads::GameThread, [WeakThis]
		{
			if (!WeakThis.IsValid()) return;
			UAbxrSubsystem* Self = WeakThis.Get();
			Self->HandleAuthCompleted(true);
		});
	};
	Callbacks.OnFailed = [WeakThis = TWeakObjectPtr(this)](const FString& Error)
	{
		AsyncTask(ENamedThreads::GameThread, [WeakThis, Error]
		{
			if (!WeakThis.IsValid()) return;
			UAbxrSubsystem* Self = WeakThis.Get();
			UE_LOG(LogAbxrLib, Warning, TEXT("Auth failed: %s"), *Error);
			Self->HandleAuthCompleted(false);
		});
	};
	
	return Callbacks;
}

void UAbxrSubsystem::Authenticate() const
{
	if (!AuthService) return;
	AuthService->Authenticate();
}

void UAbxrSubsystem::HandleAuthCompleted(const bool bSuccess)
{
	OnAuthCompleted.Broadcast(bSuccess);
	if (bIsPopupVisible)
	{
		OnPopupHidden.Broadcast();
		bIsPopupVisible = false;
	}
	
	if (!bSuccess) return;
	
	if (AuthService->GetAuthResponse().Modules.IsEmpty()) return;
	
	if (OnModuleTarget.IsBound())
	{
		if (GetDefault<UAbxrSettings>()->EnableAutoStartModules &&
			CurrentModuleIndex < AuthService->GetAuthResponse().Modules.Num())
		{
			OnModuleTarget.Broadcast(AuthService->GetAuthResponse().Modules[CurrentModuleIndex].Target);
		}
	}
	else
	{
		UE_LOG(LogAbxrLib, Error, TEXT("Need to subscribe to OnModuleTarget before running modules"));
	}
}

bool UAbxrSubsystem::StartModuleAtIndex(const int ModuleIndex)
{
	if (!AuthService || AuthService->GetAuthResponse().Modules.IsEmpty())
	{
		UE_LOG(LogAbxrLib, Error, TEXT("No modules available"));
		return false;
	}
	
	if (ModuleIndex >= AuthService->GetAuthResponse().Modules.Num() || ModuleIndex < 0)
	{
		UE_LOG(LogAbxrLib, Error, TEXT("Invalid module index - %d"), ModuleIndex);
		return false;
	}
	
	if (!OnModuleTarget.IsBound())
	{
		UE_LOG(LogAbxrLib, Error, TEXT("Need to subscribe to OnModuleTarget before running modules"));
		return false;
	}
	
	CurrentModuleIndex = ModuleIndex;
	OnModuleTarget.Broadcast(AuthService->GetAuthResponse().Modules[CurrentModuleIndex].Target);
	return true;
}

void UAbxrSubsystem::AdvanceToNextModule()
{
	CurrentModuleIndex++;
	if (CurrentModuleIndex < AuthService->GetAuthResponse().Modules.Num())
	{
		UE_LOG(LogAbxrLib, Log, TEXT("Module '%s' complete. Advancing to next module - '%s'"),
			*AuthService->GetAuthResponse().Modules[CurrentModuleIndex-1].Name,
			*AuthService->GetAuthResponse().Modules[CurrentModuleIndex].Name);
		OnModuleTarget.Broadcast(AuthService->GetAuthResponse().Modules[CurrentModuleIndex].Target);
	}
	else
	{
		UE_LOG(LogAbxrLib, Log, TEXT("All modules complete"));
		OnAllModulesCompleted.Broadcast();
	}
}

void UAbxrSubsystem::SetModule(const FString& Module, const FString& ModuleName)
{
	// Check if we're using auth-provided module targets
	if (!AuthService->GetAuthResponse().Modules.IsEmpty())
	{
		// Auth-provided modules exist - don't allow manual setting to prevent breaking module sequence
		return;
	}
		
	if (Module.IsEmpty()) return;
		
	// Directly set module metadata in super metadata, bypassing Register() check
	SuperMetaData.Add("module", Module);
			
	if (!ModuleName.IsEmpty())
	{
		SuperMetaData.Add("moduleName", ModuleName);
	}
	else
	{
		SuperMetaData.Add("moduleName", FormatModuleNameForDisplay(ModuleName));
	}
				
	// When using SetModule, we should not use moduleOrder - unset it if it was set elsewhere
	SuperMetaData.Remove("moduleOrder");
		
	SaveSuperMetaData();
}

FString UAbxrSubsystem::FormatModuleNameForDisplay(const FString& ModuleName)
{
	if (ModuleName.IsEmpty()) return ModuleName;

	// Replace underscores with spaces
	FString S = ModuleName;
	S.ReplaceInline(TEXT("_"), TEXT(" "));

	// Insert spaces on case transitions (camelCase / PascalCase)
	// Also handles "XMLParser" -> "XML Parser" by splitting when:
	//   - prev is lower and current is upper   (e.g., yM)
	//   - prev is upper, current is upper, next is lower (e.g., LP a in "XMLParser")
	FString Out;
	Out.Reserve(S.Len() * 2);

	for (int i = 0; i < S.Len(); ++i)
	{
		const TCHAR C = S[i];

		if (i > 0)
		{
			const TCHAR Prev = S[i - 1];

			const bool bPrevIsLower = FChar::IsLower(Prev);
			const bool bPrevIsUpper = FChar::IsUpper(Prev);
			const bool bCurIsUpper  = FChar::IsUpper(C);

			const bool bHasNext = i + 1 < S.Len();
			const TCHAR Next = bHasNext ? S[i + 1] : 0;
			const bool bNextIsLower = bHasNext ? FChar::IsLower(Next) : false;

			const bool bPrevIsSpace = Prev == TEXT(' ');
			if (!bPrevIsSpace && ((bPrevIsLower && bCurIsUpper) || (bPrevIsUpper && bCurIsUpper && bNextIsLower)))
			{
				Out.AppendChar(TEXT(' '));
			}
		}

		Out.AppendChar(C);
	}

	// Trim and Title-Case each word
	Out = Out.TrimStartAndEnd();

	TArray<FString> Words;
	Out.ParseIntoArray(Words, TEXT(" "), true);

	for (FString& W : Words)
	{
		if (W.IsEmpty()) continue;
		W = W.ToLower();
		W[0] = FChar::ToUpper(W[0]);
	}

	return FString::Join(Words, TEXT(" "));
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
        case ELogLevel::Debug:     LevelText = "debug";     break;
        case ELogLevel::Info:      LevelText = "info";      break;
        case ELogLevel::Warn:      LevelText = "warn";      break;
        case ELogLevel::Error:     LevelText = "error";     break;
        case ELogLevel::Critical:  LevelText = "critical";  break;
        default:                   LevelText = "info";      break;
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
	// Set module metadata using the assessment name (only if no auth-provided modules exist)
	SetModule(AssessmentName);
	
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
	if (!AuthService->GetAuthResponse().Modules.IsEmpty() && GetDefault<UAbxrSettings>()->EnableAutoAdvanceModules)
	{
		AdvanceToNextModule();
	}
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

void UAbxrSubsystem::StartNewSession() const
{
	AuthService->SetSessionId(FGuid::NewGuid().ToString());
	Authenticate();
}

void UAbxrSubsystem::Register(const FString& Key, const FString& Value, const bool Overwrite)
{
	if (IsReservedSuperMetaDataKey(Key))
	{
		const FString ErrorMessage = TEXT("Cannot register super metadata with reserved key '") + Key +
			TEXT("'. Reserved keys are: module, moduleName, moduleId, moduleOrder");
		UE_LOG(LogAbxrLib, Warning, TEXT("%s"), *ErrorMessage);
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

void UAbxrSubsystem::LoadSuperMetaData()
{
	if (!UGameplayStatics::DoesSaveGameExist(SuperMetaDataKey, 0)) return;
	
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
	// If LMS modules exist, inject current module metadata unless the event already specifies it.
	// (Data-specific metadata takes precedence.)
	if (!AuthService->GetAuthResponse().Modules.IsEmpty())
	{
		const auto [Id, Name, Target, Order] = AuthService->GetAuthResponse().Modules[CurrentModuleIndex];
		if (!Meta.Contains(TEXT("module"))) Meta.Add(TEXT("module"), Target);
		if (!Meta.Contains(TEXT("moduleName"))) Meta.Add(TEXT("moduleName"), Name);
		if (!Meta.Contains(TEXT("moduleId"))) Meta.Add(TEXT("moduleId"), Id);
		if (!Meta.Contains(TEXT("moduleOrder"))) Meta.Add(TEXT("moduleOrder"), FString::FromInt(Order));
	}

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