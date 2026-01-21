#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbxrLaserPointerActor.generated.h"

class UStaticMeshComponent;
class UWidgetInteractionComponent;

UCLASS()
class ABXRLIB_API AAbxrLaserPointerActor : public AActor
{
    GENERATED_BODY()

public:
    AAbxrLaserPointerActor();
    void Initialize(UWidgetInteractionComponent* InWidgetInteraction);
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

private:
    void UpdateBeam();
    
    UPROPERTY()
    TObjectPtr<USceneComponent> Root = nullptr;

    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> Beam = nullptr;

    // Weak ref; the subsystem owns the interaction component
    TWeakObjectPtr<UWidgetInteractionComponent> WidgetInteraction;

    float RadiusCm = 0.15f;

    // Cached mesh length in cm (derived from bounds) to scale beam accurately
    float MeshLengthCm = 100.f;
};
