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
	    Abxr->OnInputRequested = [this](const FAbxrInputRequest& Request)
	    {
	        HandleInputRequested(Request);
	    };
	}
}

void UAbxrUISubsystem::Deinitialize()
{
	if (UAbxrSubsystem* Abxr = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
	{
	    Abxr->OnInputRequested = nullptr;
	}
	HideUI();
	Super::Deinitialize();
}

AActor* UAbxrUISubsystem::SpawnActor(const EAbxrPopupType& PopupType) const
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
        Popup->PopupType = PopupType;
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

bool UAbxrUISubsystem::ShowUI()
{
    AActor* Spawned = SpawnActor(ActiveInputRequest.PopupType);
    if (!Spawned) return false;

    UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
    if (!WC) return false;

    UAbxrWidget* PopupWidget = Cast<UAbxrWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget) return false;

    ActivePopupActor = Spawned;
    ActivePopupWidget = PopupWidget;

    SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), FText::FromString(ActiveInputRequest.Prompt));
    PopupWidget->SynchronizeProperties();

    // Bind click delegate once
    PopupWidget->OnSubmitButtonClicked.RemoveAll(this);
    PopupWidget->OnSubmitButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandleSubmitClicked);
    PopupWidget->OnScanQRButtonClicked.RemoveAll(this);
    PopupWidget->OnScanQRButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandleScanQRClicked);

    if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
    {
        Subsystem->BeginUIInteraction();
    }
    
    bIsPopupVisible = true;
    OnPopupShown.Broadcast();
    return true;
}

void UAbxrUISubsystem::HideUI()
{
    if (!bIsPopupVisible) return;
    
    if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
    {
        Subsystem->EndUIInteraction();
    }

    if (AActor* A = ActivePopupActor.Get()) A->Destroy();

    ActivePopupActor.Reset();
    ActivePopupWidget.Reset();
    OnPopupHidden.Broadcast();
    bIsPopupVisible = false;
    TryProcessNextInputRequest();
}

void UAbxrUISubsystem::HandleSubmitClicked(const FText& InputText)
{
    if (UAbxrSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
    {
        Subsystem->SubmitResponse(InputText.ToString(), ActiveInputRequest);
    }
    HideUI();
}

void UAbxrUISubsystem::HandleScanQRClicked()
{
    //QRService.OnQRCodeScanned.Clear();
    //QRService.OnQRCodeScanned.AddLambda([this](const FString& DecodedText)
    //{
    //    HandleSubmitClicked(FText::FromString(DecodedText));
    //});
    //QRService.StartScan();
    //HideKeyboardUI();
}

void UAbxrUISubsystem::HandleInputRequested(const FAbxrInputRequest& Request)
{
    PendingInputRequests.Add(Request);
    TryProcessNextInputRequest();
}

void UAbxrUISubsystem::TryProcessNextInputRequest()
{
    if (bIsPopupVisible || PendingInputRequests.IsEmpty()) return;

    ActiveInputRequest = PendingInputRequests[0];
    PendingInputRequests.RemoveAt(0);
    
    if (!ShowUI())
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("ShowUI() failed"));
        TryProcessNextInputRequest();
    }
}