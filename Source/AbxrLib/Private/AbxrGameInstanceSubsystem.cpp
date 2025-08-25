#include "AbxrGameInstanceSubsystem.h"
#include "Abxr.h"
#include "Engine/GameInstance.h"

void UAbxrGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
	UAbxr::Authenticate();
}

void UAbxrGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
	bInitialized = false;
}
