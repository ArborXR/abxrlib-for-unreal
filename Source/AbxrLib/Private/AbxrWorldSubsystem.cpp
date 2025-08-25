#include "AbxrWorldSubsystem.h"

#include "Abxr.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UAbxrWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld()) return; // skip editor/preview worlds you don’t want

	EventBatcher::Init(World);
	LogBatcher::Init(World);
	TelemetryBatcher::Init(World);
	UAbxr::SetWorld(World);
	bWorldReady = true;
}

void UAbxrWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	bWorldReady = false;
}
