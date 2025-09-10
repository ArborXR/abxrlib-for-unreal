#include "AbxrGameInstanceSubsystem.h"
#include "Abxr.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"
#include "XRDMService.h"
#include "Engine/GameInstance.h"

void UAbxrGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (bInitialized) return;
	bInitialized = true;
	Super::Initialize(Collection);
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
	EventBatcher::Start();
	LogBatcher::Start();
	TelemetryBatcher::Start();
}

void UAbxrGameInstanceSubsystem::Deinitialize()
{
	EventBatcher::Stop();
	LogBatcher::Stop();
	TelemetryBatcher::Stop();
	
	Super::Deinitialize();
	bInitialized = false;
}
