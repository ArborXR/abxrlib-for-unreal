#include "UI/VRPopupLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

static void UpdatePopupInFrontOfPlayer(UWorld* World, APlayerController* PC, AActor* Popup, float DistanceCm, float FixedWorldZ)
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

AActor* UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: WorldContextObject is null"));
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: World is null"));
        return nullptr;
    }
    
    TSoftClassPtr<AActor> PopupActorPtr(FSoftObjectPath(TEXT("/AbxrLib/UI/BP_PinPadActor.BP_PinPadActor_C")));
    UClass* PopupActorClass = PopupActorPtr.LoadSynchronous();

    if (!PopupActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: Failed to load class"));
        return nullptr;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: No PlayerController"));
        return nullptr;
    }

    // Get player's current view (works even if pawn is a Blueprint-only VR pawn)
    FVector ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FVector Forward = ViewRotation.Vector();
    const float DistanceCm = 80.f;   // ~0.8m in front of HMD
    //const float ZOffsetCm  = -10.f;  // slightly below eye line

    FVector SpawnLocation = ViewLocation + Forward * DistanceCm;
    //SpawnLocation.Z += ZOffsetCm;

    // Make it face the player, but keep it level if you want
    FRotator SpawnRotation = ViewRotation;
    SpawnRotation.Pitch = 0.f;
    SpawnRotation.Roll = 0.f;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Spawned = World->SpawnActor<AActor>(PopupActorClass, SpawnLocation, SpawnRotation, Params);
    if (!Spawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: Failed to spawn popup actor"));
        return nullptr;
    }

    // Keep the popup in front of the player's view (VR-friendly "Option A").
    // Uses weak pointers so we don't keep dead objects alive.
    TWeakObjectPtr<UWorld> WeakWorld(World);
    TWeakObjectPtr<AActor> WeakPopup(Spawned);

    // Store the timer handle in a shared ref so the lambda can clear itself.
    TSharedRef<FTimerHandle, ESPMode::ThreadSafe> FollowHandle = MakeShared<FTimerHandle, ESPMode::ThreadSafe>();

    const float FixedWorldZ = SpawnLocation.Z;

    World->GetTimerManager().SetTimer(
        *FollowHandle,
        [WeakWorld, WeakPopup, FollowHandle, DistanceCm, FixedWorldZ]()
        {
            if (!WeakWorld.IsValid())
            {
                return;
            }

            UWorld* W = WeakWorld.Get();
            if (!WeakPopup.IsValid())
            {
                // Popup destroyed: stop ticking
                W->GetTimerManager().ClearTimer(*FollowHandle);
                return;
            }

            APlayerController* PC2 = UGameplayStatics::GetPlayerController(W, 0);
            if (!PC2)
            {
                return;
            }

            UpdatePopupInFrontOfPlayer(W, PC2, WeakPopup.Get(), DistanceCm, FixedWorldZ);
        },
        1.0f / 60.0f,
        true);

    // Make sure it starts at the right spot immediately
    UpdatePopupInFrontOfPlayer(World, PC, Spawned, DistanceCm, FixedWorldZ);

    return Spawned;
}
