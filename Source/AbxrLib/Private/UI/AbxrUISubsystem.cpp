#include "UI/AbxrUISubsystem.h"

#include "AbxrDisplayActor.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystems/AbxrSubsystem.h"
#include "Types/AbxrLog.h"
#include "UI/AbxrInteractionSubsystem.h"
#include "UI/AbxrWidget.h"
#include "QR/AbxrQrScanCommon.h"
#include "QR/AbxrQrScanWidget.h"
#include "QR/AbxrQrScannerCoordinator.h"

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

bool UAbxrUISubsystem::ShowPopupForRequest(const FAbxrInputRequest& Request)
{
    AActor* Spawned = SpawnActor(Request.PopupType);
    if (!Spawned) return false;

    UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
    if (!WC) return false;

    UAbxrWidget* PopupWidget = Cast<UAbxrWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget) return false;

    ActivePopupActor = Spawned;
    ActivePopupWidget = PopupWidget;
    ActiveInputRequest = Request;
    
    if (Request.PopupType == EAbxrPopupType::PollMultipleChoice)
    {
        TArray<FText> ResponseTexts;
        for (const FString& Response : Request.Responses)
        {
            ResponseTexts.Add(FText::FromString(Response));
        }

        PopupWidget->InitializePoll(ResponseTexts);
    }

    SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), FText::FromString(Request.Prompt));
    PopupWidget->SynchronizeProperties();

    PopupWidget->OnSubmitButtonClicked.RemoveAll(this);
    PopupWidget->OnSubmitButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandleSubmitClicked);
    PopupWidget->OnScanQRButtonClicked.RemoveAll(this);
    PopupWidget->OnScanQRButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandleScanQRClicked);

    if (UAbxrQrScanWidget* QrWidget = Cast<UAbxrQrScanWidget>(PopupWidget))
    {
        QrWidget->OnCancelClicked.RemoveAll(this);
        QrWidget->OnCancelClicked.AddDynamic(this, &UAbxrUISubsystem::HandleQrScanCancelled);
    }

    if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
    {
        Subsystem->BeginUIInteraction();
    }
    
    bIsPopupVisible = true;
    OnPopupShown.Broadcast();
    return true;
}

bool UAbxrUISubsystem::ShowUI()
{
    return ShowPopupForRequest(ActiveInputRequest);
}

void UAbxrUISubsystem::DestroyActivePopup(bool bEndInteraction)
{
    if (bEndInteraction)
    {
        if (UAbxrInteractionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrInteractionSubsystem>())
        {
            Subsystem->EndUIInteraction();
        }
    }

    if (AActor* A = ActivePopupActor.Get())
    {
        A->Destroy();
    }

    ActivePopupActor.Reset();
    ActivePopupWidget.Reset();
}

void UAbxrUISubsystem::HideUI()
{
    if (!bIsPopupVisible) return;

    DestroyActivePopup(true);
    OnPopupHidden.Broadcast();
    bIsPopupVisible = false;
    bHasSuspendedInputRequest = false;
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
    if (ActiveInputRequest.PopupType != EAbxrPopupType::PinPad)
    {
        return;
    }

    UAbxrQrScannerCoordinator* QrCoordinator = GetGameInstance()->GetSubsystem<UAbxrQrScannerCoordinator>();
    if (!QrCoordinator || !QrCoordinator->IsQrScanningAvailable())
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("QR scan requested but no scanner is available"));
        return;
    }

    SuspendedInputRequest = ActiveInputRequest;
    bHasSuspendedInputRequest = true;

    DestroyActivePopup(true);
    OnPopupHidden.Broadcast();
    bIsPopupVisible = false;

    FAbxrInputRequest QrRequest = ActiveInputRequest;
    QrRequest.PopupType = EAbxrPopupType::QrScan;
    QrRequest.Prompt = TEXT("Scan your QR code");

    if (!ShowPopupForRequest(QrRequest))
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to show QR popup"));
        RestoreSuspendedInputRequest();
        return;
    }

    UAbxrQrScanWidget* QrWidget = Cast<UAbxrQrScanWidget>(ActivePopupWidget.Get());
    if (!QrWidget)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("QR popup widget must inherit from UAbxrQrScanWidget"));
        DestroyActivePopup(true);
        OnPopupHidden.Broadcast();
        bIsPopupVisible = false;
        RestoreSuspendedInputRequest();
        return;
    }

    if (!QrCoordinator->BeginScan(QrWidget, [this](const FString& RawPayload, bool bCancelled)
    {
        HandleQrScanFinished(RawPayload, bCancelled);
    }))
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to begin QR scan session"));
        DestroyActivePopup(true);
        OnPopupHidden.Broadcast();
        bIsPopupVisible = false;
        RestoreSuspendedInputRequest();
    }
}

void UAbxrUISubsystem::HandleQrScanCancelled()
{
    if (UAbxrQrScannerCoordinator* QrCoordinator = GetGameInstance()->GetSubsystem<UAbxrQrScannerCoordinator>())
    {
        QrCoordinator->CancelScan();
    }
}

void UAbxrUISubsystem::HandleQrScanFinished(const FString& RawPayload, bool bCancelled)
{
    DestroyActivePopup(true);
    OnPopupHidden.Broadcast();
    bIsPopupVisible = false;

    if (bCancelled)
    {
        RestoreSuspendedInputRequest();
        return;
    }

    FString Pin;
    if (!AbxrQrScanCommon::TryExtractPinFromQrPayload(RawPayload, Pin))
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Invalid QR payload scanned: %s"), *RawPayload);
        RestoreSuspendedInputRequest();
        return;
    }

    const FAbxrInputRequest OriginalRequest = bHasSuspendedInputRequest ? SuspendedInputRequest : ActiveInputRequest;
    bHasSuspendedInputRequest = false;

    if (UAbxrSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
    {
        Subsystem->SubmitResponse(Pin, OriginalRequest);
    }

    TryProcessNextInputRequest();
}

void UAbxrUISubsystem::RestoreSuspendedInputRequest()
{
    if (!bHasSuspendedInputRequest) return;

    const FAbxrInputRequest RequestToRestore = SuspendedInputRequest;
    bHasSuspendedInputRequest = false;
    ActiveInputRequest = RequestToRestore;

    if (!ShowPopupForRequest(RequestToRestore))
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to restore PIN pad after QR flow"));
        TryProcessNextInputRequest();
    }
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
