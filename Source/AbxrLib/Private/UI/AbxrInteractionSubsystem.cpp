#include "UI/AbxrInteractionSubsystem.h"
#include "UI/AbxrLaserPointerActor.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Types/AbxrLog.h"

APawn* UAbxrInteractionSubsystem::GetLocalPawn() const
{
    const APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
    return PC ? PC->GetPawn() : nullptr;
}

void UAbxrInteractionSubsystem::BeginUIInteraction()
{
    if (bInteractionActive) return;

    if (!AcquireWidgetInteraction())
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to acquire/create WidgetInteractionComponent"));
        return;
    }

    EnsureLaserActor(ActiveHandle);
    bInteractionActive = true;
}

void UAbxrInteractionSubsystem::EndUIInteraction()
{
    if (!bInteractionActive) return;

    DestroyLaserActor(ActiveHandle);
    TeardownWidgetInteraction(ActiveHandle);
    ActiveHandle = FAbxrWidgetInteractionHandle{};
    bInteractionActive = false;
}

void UAbxrInteractionSubsystem::PressUISelect()
{
    if (!bInteractionActive || !ActiveHandle.WidgetInteraction) return;
    
    ActiveHandle.WidgetInteraction->PressPointerKey(PointerKey);
}

void UAbxrInteractionSubsystem::ReleaseUISelect()
{
    if (!bInteractionActive || !ActiveHandle.WidgetInteraction) return;
    
    ActiveHandle.WidgetInteraction->ReleasePointerKey(PointerKey);
}

bool UAbxrInteractionSubsystem::AcquireWidgetInteraction()
{
    if (UWidgetInteractionComponent* Existing = FindExistingWidgetInteraction())
    {
        ActiveHandle.WidgetInteraction = Existing;
        ConfigureWidgetInteraction(Existing, ActiveHandle);
        return true;
    }

    return false;
}

UWidgetInteractionComponent* UAbxrInteractionSubsystem::FindExistingWidgetInteraction() const
{
    const APawn* Pawn = GetLocalPawn();
    if (!Pawn)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("No local pawn found"));
        return nullptr;
    }
    
    TArray<UWidgetInteractionComponent*> Found;
    Pawn->GetComponents<UWidgetInteractionComponent>(Found);

    if (Found.Num() == 0) return nullptr;

    for (UWidgetInteractionComponent* C : Found)
    {
        if (!C) continue;
        const FString N = C->GetName();
        if (N.Contains(TEXT("Right"), ESearchCase::IgnoreCase))
        {
            return C;
        }
    }

    return Found[0];
}

void UAbxrInteractionSubsystem::ConfigureWidgetInteraction(UWidgetInteractionComponent* WIC, FAbxrWidgetInteractionHandle& Handle) const
{
    if (!WIC) return;

    // Always backup if we're going to adjust anything and later restore
    Handle.Backup.bValid = true;
    Handle.Backup.TraceChannel = WIC->TraceChannel;
    Handle.Backup.InteractionDistance = WIC->InteractionDistance;
    Handle.Backup.bShowDebug = WIC->bShowDebug;
    
    WIC->bEnableHitTesting = true;
    WIC->InteractionSource = EWidgetInteractionSource::World;
    WIC->InteractionDistance = DefaultInteractionDistance;
    WIC->TraceChannel = DefaultTraceChannel;
    WIC->bShowDebug = false;

    WIC->Activate(true);
    WIC->SetComponentTickEnabled(true);
}

void UAbxrInteractionSubsystem::TeardownWidgetInteraction(const FAbxrWidgetInteractionHandle& Handle)
{
    if (!Handle.WidgetInteraction) return;

    // Restore settings if we ever changed them
    if (Handle.Backup.bValid)
    {
        UWidgetInteractionComponent* WIC = Handle.WidgetInteraction;
        WIC->TraceChannel = Handle.Backup.TraceChannel;
        WIC->InteractionDistance = Handle.Backup.InteractionDistance;
        WIC->bShowDebug = Handle.Backup.bShowDebug;
    }
}

void UAbxrInteractionSubsystem::EnsureLaserActor(FAbxrWidgetInteractionHandle& Handle) const
{
    if (!Handle.WidgetInteraction || Handle.LaserActor) return;

    // Attach beam to whatever the widget interaction is attached to, so it follows the controller
    USceneComponent* AttachComp = Handle.WidgetInteraction->GetAttachParent();
    if (!AttachComp)
    {
        AttachComp = Handle.WidgetInteraction;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Params.Owner = AttachComp ? AttachComp->GetOwner() : nullptr;

    UWorld* World = GetGameInstance()->GetWorld();
    AAbxrLaserPointerActor* Laser = World->SpawnActor<AAbxrLaserPointerActor>(AAbxrLaserPointerActor::StaticClass(), Params);
    if (!Laser)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Failed to spawn laser actor"));
        return;
    }

    Laser->AttachToComponent(AttachComp, FAttachmentTransformRules::KeepRelativeTransform);
    Laser->SetActorRelativeLocation(FVector::ZeroVector);
    Laser->SetActorRelativeRotation(FRotator::ZeroRotator);
    Laser->Initialize(Handle.WidgetInteraction);
    Handle.LaserActor = Laser;
}

void UAbxrInteractionSubsystem::DestroyLaserActor(FAbxrWidgetInteractionHandle& Handle)
{
    if (Handle.LaserActor)
    {
        Handle.LaserActor->Destroy();
        Handle.LaserActor = nullptr;
    }
}
