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

void UAbxrWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	FTimerHandle Handle;

	// Use a weak pointer so the lambda doesn't keep a dead subsystem alive.
	TWeakObjectPtr WeakThis(this);

	InWorld.GetTimerManager().SetTimer(Handle, [&InWorld, WeakThis]
	{
		if (!WeakThis.IsValid())
		{
			return;
		}

		AActor* Spawned = UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(&InWorld, 400.f, 0.f);
		if (!Spawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("Popup spawn failed"));
			return;
		}

		UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
		if (!WC)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned popup has no WidgetComponent: %s"), *GetNameSafe(Spawned));
			return;
		}

		UVRPopupWidget* PopupWidget = Cast<UVRPopupWidget>(WC->GetUserWidgetObject());
		if (!PopupWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget is not UVRPopupWidget. Got: %s"), *GetNameSafe(WC->GetUserWidgetObject()));
			return;
		}

		// Bind C++ handler
		PopupWidget->OnPopupButtonClicked.AddDynamic(WeakThis.Get(), &UAbxrWorldSubsystem::HandlePopupClicked);

		UE_LOG(LogTemp, Log, TEXT("Bound popup click delegate. Current text: %s"),
			*PopupWidget->CurrentButtonText.ToString());
	}, 0.2f, false);
}

void UAbxrWorldSubsystem::HandlePopupClicked(const FText& ButtonText)
{
	UE_LOG(LogTemp, Warning, TEXT("Popup clicked!!! Text=%s"), *ButtonText.ToString());
}
