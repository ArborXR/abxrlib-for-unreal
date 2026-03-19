#include "AbxrLibBlueprintAPI.h"
#include "AbxrLibAPI.h"
#include "AbxrLibAPI_Internal.h"
#include "UI/AbxrUISubsystem.h"
#include "Subsystems/AbxrSubsystem.h"

namespace
{
	UAbxrLibBlueprintEvents* GAbxrLibBlueprintEventsSingleton = nullptr;
}

void UAbxrLibBlueprintEvents::TryBindToActiveSubsystem()
{
	UAbxrSubsystem* ActiveSubsystem = AbxrLib_GetActiveSubsystem();
	if (ActiveSubsystem == nullptr) return;

	UAbxrUISubsystem* ActiveUISubsystem = nullptr;
	if (const UGameInstance* GameInstance = ActiveSubsystem->GetGameInstance())
	{
		ActiveUISubsystem = GameInstance->GetSubsystem<UAbxrUISubsystem>();
	}

	const bool bSubsystemChanged = BoundSubsystem.Get() != ActiveSubsystem;
	const bool bUISubsystemChanged = BoundUISubsystem.Get() != ActiveUISubsystem;
	if (!bSubsystemChanged && !bUISubsystemChanged) return;

	if (UAbxrSubsystem* PreviousSubsystem = BoundSubsystem.Get())
	{
		PreviousSubsystem->OnAuthCompleted.RemoveDynamic(this, &UAbxrLibBlueprintEvents::HandleAuthCompleted);
		PreviousSubsystem->OnModuleTarget.RemoveDynamic(this, &UAbxrLibBlueprintEvents::HandleModuleTarget);
		PreviousSubsystem->OnAllModulesCompleted.RemoveDynamic(this, &UAbxrLibBlueprintEvents::HandleAllModulesCompleted);
	}

	if (UAbxrUISubsystem* PreviousUISubsystem = BoundUISubsystem.Get())
	{
		PreviousUISubsystem->OnPopupShown.RemoveDynamic(this, &UAbxrLibBlueprintEvents::HandlePopupShown);
		PreviousUISubsystem->OnPopupHidden.RemoveDynamic(this, &UAbxrLibBlueprintEvents::HandlePopupHidden);
	}

	BoundSubsystem = ActiveSubsystem;
	BoundUISubsystem = ActiveUISubsystem;

	ActiveSubsystem->OnAuthCompleted.AddUniqueDynamic(this, &UAbxrLibBlueprintEvents::HandleAuthCompleted);
	ActiveSubsystem->OnModuleTarget.AddUniqueDynamic(this, &UAbxrLibBlueprintEvents::HandleModuleTarget);
	ActiveSubsystem->OnAllModulesCompleted.AddUniqueDynamic(this, &UAbxrLibBlueprintEvents::HandleAllModulesCompleted);

	if (ActiveUISubsystem)
	{
		ActiveUISubsystem->OnPopupShown.AddUniqueDynamic(this, &UAbxrLibBlueprintEvents::HandlePopupShown);
		ActiveUISubsystem->OnPopupHidden.AddUniqueDynamic(this, &UAbxrLibBlueprintEvents::HandlePopupHidden);
	}
}

void UAbxrLibBlueprintEvents::HandleAuthCompleted(const bool bSuccess)
{
	OnAuthCompleted.Broadcast(bSuccess);
}

void UAbxrLibBlueprintEvents::HandlePopupShown()
{
	OnPopupShown.Broadcast();
}

void UAbxrLibBlueprintEvents::HandlePopupHidden()
{
	OnPopupHidden.Broadcast();
}

void UAbxrLibBlueprintEvents::HandleModuleTarget(const FString& ModuleTarget)
{
	OnModuleTarget.Broadcast(ModuleTarget);
}

void UAbxrLibBlueprintEvents::HandleAllModulesCompleted()
{
	OnAllModulesCompleted.Broadcast();
}

UAbxrLibBlueprintEvents* UAbxrLibBlueprintAPI::GetAbxrLibEvents()
{
	if (GAbxrLibBlueprintEventsSingleton == nullptr)
	{
		GAbxrLibBlueprintEventsSingleton = NewObject<UAbxrLibBlueprintEvents>(GetTransientPackage(), UAbxrLibBlueprintEvents::StaticClass());
		GAbxrLibBlueprintEventsSingleton->AddToRoot();
	}

	GAbxrLibBlueprintEventsSingleton->TryBindToActiveSubsystem();
	return GAbxrLibBlueprintEventsSingleton;
}

void UAbxrLibBlueprintAPI::Authenticate() { Abxr::Authenticate(); }
bool UAbxrLibBlueprintAPI::IsPopupVisible() { return Abxr::IsPopupVisible(); }
TArray<FAbxrModuleData> UAbxrLibBlueprintAPI::GetModuleList() { return Abxr::GetModuleList(); }
bool UAbxrLibBlueprintAPI::StartModuleAtIndex(const int ModuleIndex) { return Abxr::StartModuleAtIndex(ModuleIndex); }
void UAbxrLibBlueprintAPI::PollUser(const FString& Prompt, const EPollType PollType, const TArray<FString>& Responses) { return Abxr::PollUser(Prompt, PollType, Responses); }
void UAbxrLibBlueprintAPI::LogDebug(const FString& Text, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::LogDebug(Text, MutableMeta); }
void UAbxrLibBlueprintAPI::LogInfo(const FString& Text, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::LogInfo(Text, MutableMeta); }
void UAbxrLibBlueprintAPI::LogWarn(const FString& Text, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::LogWarn(Text, MutableMeta); }
void UAbxrLibBlueprintAPI::LogError(const FString& Text, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::LogError(Text, MutableMeta); }
void UAbxrLibBlueprintAPI::LogCritical(const FString& Text, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::LogCritical(Text, MutableMeta); }
void UAbxrLibBlueprintAPI::Log(const FString& Text, const ELogLevel Level, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::Log(Text, Level, MutableMeta); }
void UAbxrLibBlueprintAPI::Event(const FString& Name, const FVector& Position, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::Event(Name, Position, MutableMeta); }
void UAbxrLibBlueprintAPI::Telemetry(const FString& Name, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::Telemetry(Name, MutableMeta); }
void UAbxrLibBlueprintAPI::EventAssessmentStart(const FString& AssessmentName, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventAssessmentStart(AssessmentName, MutableMeta); }
void UAbxrLibBlueprintAPI::EventAssessmentComplete(const FString& AssessmentName, const int Score, const EEventStatus Status, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventAssessmentComplete(AssessmentName, Score, Status, MutableMeta); }
void UAbxrLibBlueprintAPI::EventObjectiveStart(const FString& ObjectiveName, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventObjectiveStart(ObjectiveName, MutableMeta); }
void UAbxrLibBlueprintAPI::EventObjectiveComplete(const FString& ObjectiveName, const int Score, const EEventStatus Status, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventObjectiveComplete(ObjectiveName, Score, Status, MutableMeta); }
void UAbxrLibBlueprintAPI::EventInteractionStart(const FString& InteractionName, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventInteractionStart(InteractionName, MutableMeta); }
void UAbxrLibBlueprintAPI::EventInteractionComplete(const FString& InteractionName, const EInteractionType InteractionType, const FString& Response, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventInteractionComplete(InteractionName, InteractionType, Response, MutableMeta); }
void UAbxrLibBlueprintAPI::EventLevelStart(const FString& LevelName, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventLevelStart(LevelName, MutableMeta); }
void UAbxrLibBlueprintAPI::EventLevelComplete(const FString& LevelName, const int Score, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventLevelComplete(LevelName, Score, MutableMeta); }
void UAbxrLibBlueprintAPI::EventCritical(const FString& Label, const TMap<FString, FString>& Meta) { TMap<FString, FString> MutableMeta = Meta; Abxr::EventCritical(Label, MutableMeta); }
FString UAbxrLibBlueprintAPI::GetDeviceId() { return Abxr::GetDeviceId(); }
FString UAbxrLibBlueprintAPI::GetDeviceSerial() { return Abxr::GetDeviceSerial(); }
FString UAbxrLibBlueprintAPI::GetDeviceTitle() { return Abxr::GetDeviceTitle(); }
TArray<FString> UAbxrLibBlueprintAPI::GetDeviceTags() { return Abxr::GetDeviceTags(); }
FString UAbxrLibBlueprintAPI::GetOrgId() { return Abxr::GetOrgId(); }
FString UAbxrLibBlueprintAPI::GetOrgTitle() { return Abxr::GetOrgTitle(); }
FString UAbxrLibBlueprintAPI::GetOrgSlug() { return Abxr::GetOrgSlug(); }
FString UAbxrLibBlueprintAPI::GetMacAddressFixed() { return Abxr::GetMacAddressFixed(); }
FString UAbxrLibBlueprintAPI::GetMacAddressRandom() { return Abxr::GetMacAddressRandom(); }
bool UAbxrLibBlueprintAPI::GetIsAuthenticated() { return Abxr::GetIsAuthenticated(); }
FString UAbxrLibBlueprintAPI::GetAccessToken() { return Abxr::GetAccessToken(); }
FString UAbxrLibBlueprintAPI::GetRefreshToken() { return Abxr::GetRefreshToken(); }
FDateTime UAbxrLibBlueprintAPI::GetExpiresDateUtc() { return Abxr::GetExpiresDateUtc(); }
FString UAbxrLibBlueprintAPI::GetFingerprint() { return Abxr::GetFingerprint(); }
void UAbxrLibBlueprintAPI::StartNewSession() { Abxr::StartNewSession(); }
TMap<FString, FString> UAbxrLibBlueprintAPI::GetUserData() { return Abxr::GetUserData(); }
void UAbxrLibBlueprintAPI::Register(const FString& Key, const FString& Value) { Abxr::Register(Key, Value); }
void UAbxrLibBlueprintAPI::RegisterOnce(const FString& Key, const FString& Value) { Abxr::RegisterOnce(Key, Value); }
void UAbxrLibBlueprintAPI::Unregister(const FString& Key) { Abxr::Unregister(Key); }
void UAbxrLibBlueprintAPI::Reset() { Abxr::Reset(); }
TMap<FString, FString> UAbxrLibBlueprintAPI::GetSuperMetaData() { return Abxr::GetSuperMetaData(); }
void UAbxrLibBlueprintAPI::LoadSuperMetaData() { Abxr::LoadSuperMetaData(); }
