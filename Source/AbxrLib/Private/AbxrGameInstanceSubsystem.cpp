#include "AbxrGameInstanceSubsystem.h"
#include "Abxr.h"
#include "AbxrLibConfiguration.h"
#include "DataBatcher.h"
#include "LevelTracker.h"
#include "XRDMService.h"
#include "Engine/Engine.h"

void UAbxrGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, 
		&UAbxrGameInstanceSubsystem::OnPostLoadMapWithWorld
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
	if (GetDefault<UAbxrLibConfiguration>()->EnableAutoStartAuth)
	{
		if (GetDefault<UAbxrLibConfiguration>()->AuthenticationStartDelay > 0)
		{
			UAbxr::GetCurrentWorld()->GetTimerManager().SetTimer(
					AuthenticationTimerHandle,
					this,
					&UAbxrGameInstanceSubsystem::StartAuthAfterDelay,
					GetDefault<UAbxrLibConfiguration>()->AuthenticationStartDelay,
					false // don't loop
				);
		}
		else
		{
			UAbxr::Authenticate();
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AbxrLib: Auto-start authentication is disabled. Call UAbxr::Authenticate() manually when ready."));
	}
	
	DataBatcher::Start();
}

void UAbxrGameInstanceSubsystem::Deinitialize()
{
	DataBatcher::Stop();
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	
	Super::Deinitialize();
	bInitialized = false;
}

void UAbxrGameInstanceSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!LoadedWorld) return;
    
	const FString NewLevelName = LoadedWorld->GetName();
	if (NewLevelName != LevelTracker::GetCurrentLevel())
	{
		LevelTracker::SetCurrentLevel(NewLevelName);
		if (GetDefault<UAbxrLibConfiguration>()->EnableSceneEvents)
		{
			TMap<FString, FString> Meta;
			Meta.Add(TEXT("Scene Name"), NewLevelName);
			UAbxr::Event(TEXT("Scene Changed"), Meta);
		}
	}
}

void UAbxrGameInstanceSubsystem::StartAuthAfterDelay()
{
	UAbxr::Authenticate();
}
