#include "UI/AbxrUISubsystem.h"
#include "AbxrDisplayActor.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystems/AbxrSubsystem.h"
#include "Types/AbxrLog.h"
#include "UI/AbxrInteractionSubsystem.h"
#include "UI/AbxrWidget.h"
#include "Services/Platform/QR/AbxrQRService.h"
#include "Engine/Texture2D.h"
#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
#endif

void UAbxrUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	QRService = MakeUnique<FAbxrQRService>();
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
	if (QRService)
	{
		QRService->StopScan();
		QRService.Reset();
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
    
    if (ActiveInputRequest.PopupType == EAbxrPopupType::PollMultipleChoice)
    {
        TArray<FText> ResponseTexts;
        for (const FString& Response : ActiveInputRequest.Responses)
        {
            ResponseTexts.Add(FText::FromString(Response));
        }

        PopupWidget->InitializePoll(ResponseTexts);
    }

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

    if (QRService) QRService->StopScan();
    
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
    if (!QRService) return;

#if PLATFORM_ANDROID
    static const FString CameraPermission = TEXT("android.permission.CAMERA");
    static const FString HeadsetCameraPermission = TEXT("horizonos.permission.HEADSET_CAMERA");
    const bool bCameraGranted = UAndroidPermissionFunctionLibrary::CheckPermission(CameraPermission);
    const bool bHeadsetGranted = UAndroidPermissionFunctionLibrary::CheckPermission(HeadsetCameraPermission);
    if (!bCameraGranted || !bHeadsetGranted)
    {
        TArray<FString> Permissions;
        Permissions.Add(CameraPermission);
        Permissions.Add(HeadsetCameraPermission);

        if (UAndroidPermissionCallbackProxy* Proxy = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions))
        {
            Proxy->OnPermissionsGrantedDynamicDelegate.AddDynamic(this, &UAbxrUISubsystem::HandleCameraPermissionResult);
        }
        else
        {
            if (UAbxrWidget* PopupWidget = ActivePopupWidget.Get())
            {
                const FString NewPrompt = TEXT("Unable to request camera permission.\n") + ActiveInputRequest.Prompt;
                SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), FText::FromString(NewPrompt));
                PopupWidget->SynchronizeProperties();
            }
        }
        return;
    }
#endif

    StartQRScannerUI();
}

void UAbxrUISubsystem::StartQRScannerUI()
{
    if (!QRService) return;

    AActor* PreviousActor = ActivePopupActor.Get();

    AActor* Spawned = SpawnActor(EAbxrPopupType::QRScanner);
    if (!Spawned) return;

    UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
    if (!WC)
    {
        Spawned->Destroy();
        return;
    }

    UAbxrWidget* PopupWidget = Cast<UAbxrWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget)
    {
        Spawned->Destroy();
        return;
    }

    if (PreviousActor) PreviousActor->Destroy();

    ActivePopupActor = Spawned;
    ActivePopupWidget = PopupWidget;
    QRPreviewTexture = nullptr;

    SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), FText::FromString(TEXT("Look at QR code")));
    PopupWidget->SynchronizeProperties();

    PopupWidget->OnCancelQRButtonClicked.RemoveAll(this);
    PopupWidget->OnCancelQRButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandleQRCancelled);

    QRService->OnQRPreviewFrame.Clear();
    QRService->OnQRPreviewFrame.AddUObject(this, &UAbxrUISubsystem::HandleQRPreviewFrame);
    QRService->OnQRScanFailed.Clear();
    QRService->OnQRScanFailed.AddUObject(this, &UAbxrUISubsystem::HandleQRFailed);
    QRService->OnQRScanCancelled.Clear();
    QRService->OnQRScanCancelled.AddUObject(this, &UAbxrUISubsystem::HandleQRCancelled);
    QRService->OnQRCodeScanned.Clear();
    QRService->OnQRCodeScanned.AddLambda([this](const FString& DecodedText)
    {
        HandleSubmitClicked(FText::FromString(DecodedText));
    });

    if (!QRService->StartScan())
    {
        HandleQRFailed(TEXT("Failed to start QR scan"));
        return;
    }

    bIsPopupVisible = true;
}

void UAbxrUISubsystem::HandleCameraPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults)
{
    bool bCameraGranted = false;
    bool bHeadsetGranted = false;

    for (int Index = 0; Index < Permissions.Num() && Index < GrantResults.Num(); ++Index)
    {
        if (Permissions[Index] == TEXT("android.permission.CAMERA"))
        {
            bCameraGranted = GrantResults[Index];
        }
        else if (Permissions[Index] == TEXT("horizonos.permission.HEADSET_CAMERA"))
        {
            bHeadsetGranted = GrantResults[Index];
        }
    }

    if (!bCameraGranted || !bHeadsetGranted)
    {
        if (UAbxrWidget* PopupWidget = ActivePopupWidget.Get())
        {
            const FString NewPrompt = TEXT("No camera permission.\n") + ActiveInputRequest.Prompt;
            SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), FText::FromString(NewPrompt));
            PopupWidget->SynchronizeProperties();
        }
        return;
    }

    StartQRScannerUI();
}

void UAbxrUISubsystem::HandleQRCancelled()
{
    if (QRService) QRService->StopScan();

    if (AActor* A = ActivePopupActor.Get()) A->Destroy();

    ActivePopupActor.Reset();
    ActivePopupWidget.Reset();
    QRPreviewTexture = nullptr;
    bIsPopupVisible = false;

    ShowUI();
}

void UAbxrUISubsystem::HandleQRFailed(const FString& Error)
{
    UE_LOG(LogAbxrLib, Warning, TEXT("QR scan failed: %s"), *Error);
    HandleQRCancelled();
}

void UAbxrUISubsystem::HandleQRPreviewFrame(const TArray<uint8>& Pixels, const int32 Width, const int32 Height)
{
    if (Pixels.Num() != Width * Height * 4) return;

    if (!QRPreviewTexture || QRPreviewTexture->GetSizeX() != Width || QRPreviewTexture->GetSizeY() != Height)
    {
        QRPreviewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
        QRPreviewTexture->SRGB = true;
        QRPreviewTexture->NeverStream = true;
        QRPreviewTexture->Filter = TF_Bilinear;
        QRPreviewTexture->UpdateResource();
    }

    TArray<uint8> Bgra;
    Bgra.SetNumUninitialized(Pixels.Num());
    for (int32 Index = 0; Index < Width * Height; ++Index)
    {
        const int32 Src = Index * 4;
        const uint8 R = Pixels[Src + 0];
        const uint8 G = Pixels[Src + 1];
        const uint8 B = Pixels[Src + 2];
        const uint8 A = Pixels[Src + 3];
        Bgra[Src + 0] = B;
        Bgra[Src + 1] = G;
        Bgra[Src + 2] = R;
        Bgra[Src + 3] = A;
    }

    const FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);

    uint8* UploadData = new uint8[Bgra.Num()];
    FMemory::Memcpy(UploadData, Bgra.GetData(), Bgra.Num());

    QRPreviewTexture->UpdateTextureRegions(0, 1, Region, Width * 4, 4, UploadData,
        [](const uint8* SrcData, const FUpdateTextureRegion2D* SrcRegions)
        {
            delete[] SrcData;
            delete SrcRegions;
        });

    if (UAbxrWidget* PopupWidget = ActivePopupWidget.Get())
    {
        PopupWidget->SetQRPreviewTexture(QRPreviewTexture);
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