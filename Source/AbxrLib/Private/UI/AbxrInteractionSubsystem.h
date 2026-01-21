#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputCoreTypes.h"
#include "AbxrInteractionSubsystem.generated.h"

class UWidgetInteractionComponent;
class AAbxrLaserPointerActor;

USTRUCT()
struct FAbxrWidgetInteractionBackup
{
    GENERATED_BODY()

    bool bValid = false;

    TEnumAsByte<ECollisionChannel> TraceChannel;
    float InteractionDistance;
    bool bShowDebug;
};

USTRUCT()
struct FAbxrWidgetInteractionHandle
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UWidgetInteractionComponent> WidgetInteraction = nullptr;

    // If we found an existing one and changed settings, store backup
    FAbxrWidgetInteractionBackup Backup;

    // Laser actor we spawned (always ours; we destroy it on End)
    UPROPERTY()
    TObjectPtr<AAbxrLaserPointerActor> LaserActor = nullptr;
};

UCLASS(BlueprintType)
class ABXRLIB_API UAbxrInteractionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void BeginUIInteraction();
    
    UFUNCTION()
    void EndUIInteraction();
    
    UFUNCTION(BlueprintCallable, Category="Abxr|Interaction")
    void PressUISelect();

    UFUNCTION(BlueprintCallable, Category="Abxr|Interaction")
    void ReleaseUISelect();

private:
    APawn* GetLocalPawn() const;

    bool AcquireWidgetInteraction();
    UWidgetInteractionComponent* FindExistingWidgetInteraction() const;
    void ConfigureWidgetInteraction(UWidgetInteractionComponent* WIC, FAbxrWidgetInteractionHandle& Handle) const;
    static void TeardownWidgetInteraction(const FAbxrWidgetInteractionHandle& Handle);

    void EnsureLaserActor(FAbxrWidgetInteractionHandle& Handle) const;
    static void DestroyLaserActor(FAbxrWidgetInteractionHandle& Handle);
    
    bool bInteractionActive = false;
    FAbxrWidgetInteractionHandle ActiveHandle;
    
    float DefaultInteractionDistance = 700.f;
    TEnumAsByte<ECollisionChannel> DefaultTraceChannel = ECC_Visibility;
    FKey PointerKey = EKeys::LeftMouseButton;  // used for "click"
};
