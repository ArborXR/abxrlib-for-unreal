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
	UAbxr::Authenticate();
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
    
	FString NewLevelName = LoadedWorld->GetName();
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
