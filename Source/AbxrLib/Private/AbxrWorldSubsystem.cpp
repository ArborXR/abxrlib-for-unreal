#include "AbxrWorldSubsystem.h"
#include "Abxr.h"
#include "VRPopupLibrary.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UAbxrWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld()) return; // skip editor/preview worlds you don’t want
	
	UAbxr::SetWorld(World);
	bWorldReady = true;
}

void UAbxrWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	bWorldReady = false;
}

void UAbxrWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	FTimerHandle Handle;
	InWorld.GetTimerManager().SetTimer(Handle, [&InWorld]
	{
		UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(&InWorld, 400.f, 0.f);
	}, 0.2f, false);
}