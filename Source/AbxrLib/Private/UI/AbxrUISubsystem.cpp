#include "UI/AbxrUISubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/AbxrSubsystem.h"
#include "Types/AbxrLog.h"
#include "UI/AbxrInteractionSubsystem.h"
#include "UI/AbxrWidget.h"
#include "Services/Config/AbxrSettings.h"

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

static void UpdateInFrontOfPlayer(const UWorld* World, const APlayerController* PC, AActor* Popup, const float DistanceCm, const float FixedWorldZ)
{
    if (!World || !PC || !Popup) return;

    FVector ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // Use yaw-only so pitch doesn't move it up/down or change perceived distance
    const FRotator YawRot(0.f, ViewRotation.Yaw, 0.f);
    const FVector Forward = YawRot.Vector();

    FVector TargetLoc = ViewLocation + Forward * DistanceCm;

    // Lock height (no head bob)
    TargetLoc.Z = FixedWorldZ;

    // Face the player (yaw-only)
    const FRotator TargetRot(0.f, ViewRotation.Yaw, 0.f);

    Popup->SetActorLocationAndRotation(TargetLoc, TargetRot, false, nullptr, ETeleportType::TeleportPhysics);
}

AActor* UAbxrUISubsystem::SpawnInFrontOfPlayer(UWorld* World, const FString& Type)
{
    TSoftClassPtr<AActor> ActorPtr;
    if (Type == TEXT("assessmentPin"))
    {
        ActorPtr = FSoftObjectPath(TEXT("/AbxrLib/UI/BP_PinPadActor.BP_PinPadActor_C"));
    }
    else
    {
        ActorPtr = FSoftObjectPath(TEXT("/AbxrLib/UI/BP_KeyboardActor.BP_KeyboardActor_C"));
    }
    
    UClass* PopupActorClass = ActorPtr.LoadSynchronous();
    if (!PopupActorClass)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to load class"));
        return nullptr;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("No PlayerController"));
        return nullptr;
    }

    // Get player's current view (works even if pawn is a Blueprint-only VR pawn)
    FVector ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FVector Forward = ViewRotation.Vector();
    constexpr float DistanceCm = 80.f;  // ~0.8m in front of HMD
    constexpr float ZOffsetCm  = -5.f;  // slightly below eye line

    FVector SpawnLocation = ViewLocation + Forward * DistanceCm;
    SpawnLocation.Z += ZOffsetCm;

    // Make it face the player
    FRotator SpawnRotation = ViewRotation;
    SpawnRotation.Pitch = 0.f;
    SpawnRotation.Roll = 0.f;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Spawned = World->SpawnActor<AActor>(PopupActorClass, SpawnLocation, SpawnRotation, Params);
    if (!Spawned)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to spawn popup actor"));
        return nullptr;
    }
	
    // Use weak pointers so we don't keep dead objects alive
    TWeakObjectPtr WeakWorld(World);
    TWeakObjectPtr WeakPopup(Spawned);

    // Store the timer handle in a shared ref so the lambda can clear itself
    TSharedRef<FTimerHandle> FollowHandle = MakeShared<FTimerHandle, ESPMode::ThreadSafe>();

    const float FixedWorldZ = SpawnLocation.Z;

    World->GetTimerManager().SetTimer(
        *FollowHandle,
        [WeakWorld, WeakPopup, FollowHandle, DistanceCm, FixedWorldZ]
        {
            if (!WeakWorld.IsValid()) return;

            const UWorld* W = dynamic_cast<UWorld*>(WeakWorld.Get());
            if (!WeakPopup.IsValid())
            {
                // Popup destroyed: stop ticking
                W->GetTimerManager().ClearTimer(*FollowHandle);
                return;
            }

            const APlayerController* PC2 = UGameplayStatics::GetPlayerController(W, 0);
            if (!PC2) return;

            UpdateInFrontOfPlayer(W, PC2, dynamic_cast<AActor*>(WeakPopup.Get()), DistanceCm, FixedWorldZ);
        },
        1.0f / 60.0f,
        true);

    // Make sure it starts at the right spot
    UpdateInFrontOfPlayer(World, PC, Spawned, DistanceCm, FixedWorldZ);

    return Spawned;
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
    UWorld* World = GetWorld();

    AActor* Spawned = SpawnInFrontOfPlayer(World, Type);
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

    // If the user has configured a custom widget class in Project Settings, swap it in
    const bool bIsPinPad = Type == TEXT("assessmentPin");
    const UAbxrSettings* Settings = GetDefault<UAbxrSettings>();
    if (Settings)
    {
        const TSoftClassPtr<UUserWidget>& CustomWidgetClassPtr = bIsPinPad
            ? Settings->CustomPinPadWidgetClass
            : Settings->CustomKeyboardWidgetClass;
        if (!CustomWidgetClassPtr.IsNull())
        {
            if (UClass* CustomWidgetClass = CustomWidgetClassPtr.LoadSynchronous())
            {
                // Verify it actually inherits from UVRPopupWidget before swapping
                if (CustomWidgetClass->IsChildOf(UAbxrWidget::StaticClass()))
                {
                    WC->SetWidgetClass(CustomWidgetClass);
                    UE_LOG(LogAbxrLib, Log, TEXT("Using custom %s widget class: %s"),
                        bIsPinPad ? TEXT("PIN pad") : TEXT("keyboard"),
                        *CustomWidgetClass->GetName());
                }
            }
            else
            {
                UE_LOG(LogAbxrLib, Warning, TEXT("Failed to load custom widget class — using default"));
            }
        }
    }

    UAbxrWidget* PopupWidget = Cast<UAbxrWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Widget is not UVRPopupWidget"));
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