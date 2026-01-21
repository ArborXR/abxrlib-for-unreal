#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputCoreTypes.h"
#include "Types/AbxrTypes.h"
#include "AbxrInteractionSubsystem.generated.h"

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
