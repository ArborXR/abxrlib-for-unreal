#include "UI/AbxrUISubsystem.h"
#include "AbxrDisplayActor.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystems/AbxrSubsystem.h"
#include "Types/AbxrLog.h"
#include "UI/AbxrInteractionSubsystem.h"
#include "UI/AbxrWidget.h"

void UAbxrUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (UAbxrSubsystem* Abxr = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
	{
		Abxr->OnInputRequested.AddDynamic(this, &UAbxrUISubsystem::HandleInputRequested);
	}
}

void UAbxrUISubsystem::Deinitialize()
{
	if (UAbxrSubsystem* Abxr = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
	{
		Abxr->OnInputRequested.RemoveAll(this);
	}
	HideKeyboardUI();
	Super::Deinitialize();
}

AActor* UAbxrUISubsystem::SpawnActor(const FString& Type) const
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    const APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return nullptr;

    FVector CamLoc;
    FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FTransform SpawnTransform(FRotator(0, CamRot.Yaw + 180.f, 0), CamLoc + CamRot.Vector() * 140.0f);
    AAbxrDisplayActor* Popup = World->SpawnActorDeferred<AAbxrDisplayActor>(
        AAbxrDisplayActor::StaticClass(), SpawnTransform, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (Popup)
    {
        Popup->PopupType = Type;
        Popup->FinishSpawning(SpawnTransform);
    }

    return Popup;
}

static bool SetUserWidgetTextProperty(UUserWidget* Widget, const FName PropertyName, const FText& Value)
{
    if (!Widget) return false;
    if (FProperty* Prop = Widget->GetClass()->FindPropertyByName(PropertyName))
    {
        if (const FTextProperty* TextProp = CastField<FTextProperty>(Prop))
        {
            TextProp->SetPropertyValue_InContainer(Widget, Value);
            return true;
        }
    }
    return false;
}

void UAbxrUISubsystem::ShowKeyboardUI(const FText& Prompt, const FString& Type)
{
    AActor* Spawned = SpawnActor(Type);
    if (!Spawned)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Popup spawn failed"));
        return;
    }

    UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
    if (!WC)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Spawned popup has no WidgetComponent"));
        return;
    }

    UAbxrWidget* PopupWidget = Cast<UAbxrWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Widget is not UAbxrWidget"));
        return;
    }

    ActivePopupActor = Spawned;
    ActivePopupWidget = PopupWidget;

    SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), Prompt);
    PopupWidget->SynchronizeProperties();

    // Bind click delegate once
    PopupWidget->OnSubmitButtonClicked.RemoveAll(this);
    PopupWidget->OnSubmitButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandlePopupClicked);

    if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
    {
        Subsystem->BeginUIInteraction();
    }
}

void UAbxrUISubsystem::HideKeyboardUI()
{
    if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
    {
        Subsystem->EndUIInteraction();
    }
    
    if (AActor* A = ActivePopupActor.Get())
    {
        A->Destroy();
    }
    ActivePopupActor.Reset();
    ActivePopupWidget.Reset();
}

void UAbxrUISubsystem::HandlePopupClicked(const FText& InputText)
{
    if (const UAbxrSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
    {
        Subsystem->SubmitInput(InputText.ToString());
    }
    HideKeyboardUI();
}

void UAbxrUISubsystem::HandleInputRequested(const FAbxrAuthMechanism& Request)
{
	ShowKeyboardUI(FText::FromString(Request.Prompt), Request.Type);
}