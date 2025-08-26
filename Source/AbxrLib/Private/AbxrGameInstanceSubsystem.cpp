#include "AbxrGameInstanceSubsystem.h"
#include "Abxr.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"
#include "Engine/GameInstance.h"

void UAbxrGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
	UAbxr::Authenticate();
	EventBatcher::Start();
	LogBatcher::Start();
	TelemetryBatcher::Start();
}

void UAbxrGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
	EventBatcher::Stop();
	LogBatcher::Stop();
	TelemetryBatcher::Stop();
	bInitialized = false;
}
