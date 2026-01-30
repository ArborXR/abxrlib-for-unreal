#include "UI/AbxrUISubsystem.h"
#include "Services/Auth/AbxrAuthService.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/AbxrSubsystem.h"
#include "Types/AbxrLog.h"
#include "UI/AbxrInteractionSubsystem.h"
#include "UI/VRPopupWidget.h"

static void UpdatePopupInFrontOfPlayer(const UWorld* World, const APlayerController* PC, AActor* Popup, const float DistanceCm, const float FixedWorldZ)
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

AActor* UAbxrUISubsystem::SpawnPopupInFrontOfPlayer(UWorld* World)
{
    TSoftClassPtr<AActor> PopupActorPtr(FSoftObjectPath(TEXT("/AbxrLib/UI/BP_PinPadActor.BP_PinPadActor_C")));
    UClass* PopupActorClass = PopupActorPtr.LoadSynchronous();  // TODO maybe do this when app loads
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
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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

            const UWorld* W = WeakWorld.Get();
            if (!WeakPopup.IsValid())
            {
                // Popup destroyed: stop ticking
                W->GetTimerManager().ClearTimer(*FollowHandle);
                return;
            }

            const APlayerController* PC2 = UGameplayStatics::GetPlayerController(W, 0);
            if (!PC2) return;

            UpdatePopupInFrontOfPlayer(W, PC2, WeakPopup.Get(), DistanceCm, FixedWorldZ);
        },
        1.0f / 60.0f,
        true);

    // Make sure it starts at the right spot
    UpdatePopupInFrontOfPlayer(World, PC, Spawned, DistanceCm, FixedWorldZ);

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

void UAbxrUISubsystem::ShowKeyboardUI(const FText& PromptText)
{
    UWorld* World = GetWorld();

    // Spawn once (no timer needed unless you truly require a delay)
    AActor* Spawned = SpawnPopupInFrontOfPlayer(World);
    if (!Spawned)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Popup spawn failed"));
        return;
    }

    const UWidgetComponent* WC = Spawned->FindComponentByClass<UWidgetComponent>();
    if (!WC)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Spawned popup has no WidgetComponent"));
        return;
    }

    UVRPopupWidget* PopupWidget = Cast<UVRPopupWidget>(WC->GetUserWidgetObject());
    if (!PopupWidget)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Widget is not UVRPopupWidget"));
        return;
    }
    
    ActivePopupActor = Spawned;
    ActivePopupWidget = PopupWidget;
    
    SetUserWidgetTextProperty(PopupWidget, TEXT("PromptText"), PromptText);
    PopupWidget->SynchronizeProperties();

    // Bind click delegate once
    PopupWidget->OnPopupButtonClicked.RemoveAll(this);
    PopupWidget->OnPopupButtonClicked.AddDynamic(this, &UAbxrUISubsystem::HandlePopupClicked);

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
    if (UAbxrSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UAbxrSubsystem>())
    {
        Subsystem->GetAuthService()->KeyboardAuthenticate(InputText.ToString());
    }
    HideKeyboardUI();
}