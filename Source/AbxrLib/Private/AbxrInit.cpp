#include "AbxrInit.h"
#include "Abxr.h"
#include "EventBatcher.h"
#include "InputDialogWidget.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"
#include "Engine/World.h"
#include "Logging/LogMacros.h"

void UAbxrInit::Init()
{
	Super::Init();
	
	UAbxr::Authenticate();
	
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

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAbxrInit::AskForName, 1.5f, false);
}

void UAbxrInit::AskForName()
{
	if (auto* Dialog = UInputDialogWidget::ShowDialog(
				this,
				FText::FromString("Please enter your name: "),
				FText::FromString("Type here...")))
	{
		Dialog->OnAccepted.AddLambda([](const FString& Text)
		{
			UE_LOG(LogTemp, Log, TEXT("User typed: %s"), *Text);
		});
	}
}

void UAbxrInit::MyRepeatingFunction()
{
	UAbxr::Event("someEvent");
	UAbxr::LogDebug("debug logging");
	UAbxr::TelemetryEntry("someTelemetry");
}
