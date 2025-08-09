#include "AbxrInit.h"
#include "Abxr.h"
#include "EventBatcher.h"
#include "Engine/World.h"
#include "Logging/LogMacros.h"

void UAbxrInit::Init()
{
	Super::Init();
	
	UAbxr::Authenticate();
	
	UWorld* World = GetWorld();
	EventBatcher::Init(World);
	
	// Start a repeating timer
	World->GetTimerManager().SetTimer(
		MyRepeatingTimer,
		this,
		&UAbxrInit::MyRepeatingFunction,
		3.0f,   // Interval in seconds
		true    // Loop
	);
}

void UAbxrInit::MyRepeatingFunction()
{
	UAbxr::Event("someEvent");
}
