#include "AbxrWorldSubsystem.h"
#include "Abxr.h"
#include "VRPopupLibrary.h"
#include "Components/WidgetComponent.h"
#include "VRPopupWidget.h"
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

// In UAbxrWorldSubsystem.cpp
void UAbxrWorldSubsystem::ShowKeyboardUI(float Distance, float VerticalOffset)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowKeyboardUI: No World"));
		return;
	}

	// Keep handle as a member if you want to cancel later; local is fine if you don't care
	FTimerHandle Handle;
	TWeakObjectPtr WeakThis(this);

	World->GetTimerManager().SetTimer(Handle, [World, WeakThis, Distance, VerticalOffset]()
	{
		if (!WeakThis.IsValid() || !World) return;

		AActor* Spawned = UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(World, Distance, VerticalOffset);
		if (!Spawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowKeyboardUI: Popup spawn failed"));
			return;
		}

		UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
		if (!WC)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowKeyboardUI: Spawned popup has no WidgetComponent: %s"), *GetNameSafe(Spawned));
			return;
		}

		UVRPopupWidget* PopupWidget = Cast<UVRPopupWidget>(WC->GetUserWidgetObject());
		if (!PopupWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowKeyboardUI: Widget is not UVRPopupWidget. Got: %s"), *GetNameSafe(WC->GetUserWidgetObject()));
			return;
		}

		PopupWidget->OnPopupButtonClicked.RemoveAll(WeakThis.Get()); // avoid double-binding
		PopupWidget->OnPopupButtonClicked.AddDynamic(WeakThis.Get(), &UAbxrWorldSubsystem::HandlePopupClicked);

		UE_LOG(LogTemp, Log, TEXT("ShowKeyboardUI: Bound popup click delegate. Current text: %s"),
			*PopupWidget->CurrentButtonText.ToString());

	}, 0.2f, false);
}

void UAbxrWorldSubsystem::HandlePopupClicked(const FText& ButtonText)
{
	UE_LOG(LogTemp, Warning, TEXT("Popup clicked!!! Text=%s"), *ButtonText.ToString());
}
