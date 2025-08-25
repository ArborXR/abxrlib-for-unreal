#include "AbxrInit.h"
#include "Abxr.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"
#include "Engine/World.h"
#include "Logging/LogMacros.h"

void UAbxrInit::Init()
{
	Super::Init();
	
	//UAbxr::Authenticate();
	
	UWorld* World = GetWorld();
	EventBatcher::Init(World);
	LogBatcher::Init(World);
	TelemetryBatcher::Init(World);
	
	// Start a repeating timer
	World->GetTimerManager().SetTimer(
		MyRepeatingTimer,
		this,
		&UAbxrInit::MyRepeatingFunction,
		3.0f,   // Interval in seconds
		true    // Loop
	);

	//UAbxr::SetWorld(World);
}

void UAbxrInit::MyRepeatingFunction()
{
	UAbxr::Event("someEvent");
	UAbxr::LogDebug("debug logging");
	UAbxr::TelemetryEntry("someTelemetry");
}
