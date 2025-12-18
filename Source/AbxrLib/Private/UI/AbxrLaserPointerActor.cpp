#include "UI/AbxrLaserPointerActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AAbxrLaserPointerActor::AAbxrLaserPointerActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Beam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
    Beam->SetupAttachment(Root);

    Beam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Beam->SetGenerateOverlapEvents(false);
    Beam->CastShadow = false;
    Beam->SetReceivesDecals(false);
    Beam->SetHiddenInGame(false);
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderMesh.Succeeded())
    {
        Beam->SetStaticMesh(CylinderMesh.Object);
        
        const FBoxSphereBounds B = CylinderMesh.Object->GetBounds();
        MeshLengthCm = FMath::Max(1.f, B.BoxExtent.Z * 2.f);
    }
    
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BasicMat.Succeeded())
    {
        Beam->SetMaterial(0, BasicMat.Object);
    }
}

void AAbxrLaserPointerActor::Initialize(UWidgetInteractionComponent* InWidgetInteraction)
{
    WidgetInteraction = InWidgetInteraction;
}

void AAbxrLaserPointerActor::BeginPlay()
{
    Super::BeginPlay();
    UpdateBeam();
}

void AAbxrLaserPointerActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateBeam();
}

void AAbxrLaserPointerActor::UpdateBeam()
{
    if (!Beam) return;

    UWidgetInteractionComponent* WIC = WidgetInteraction.Get();
    if (!WIC)
    {
        // If the interaction component is gone, hide beam
        Beam->SetVisibility(false);
        return;
    }

    Beam->SetVisibility(true);

    const FVector Start = WIC->GetComponentLocation();
    const FVector Dir = WIC->GetForwardVector();

    const float MaxDist = WIC->InteractionDistance;
    const FVector MaxEnd = Start + Dir * MaxDist;

    // Prefer the nearest of: world geometry hit OR widget interaction hit
    float BestDist = MaxDist;
    FVector End = MaxEnd;

    // World occlusion (walls, props, etc.) so the beam doesn't go through geometry
    if (UWorld* World = GetWorld())
    {
        FHitResult WorldHit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(AbxrLaserWorldTrace), true);
        Params.AddIgnoredActor(this);
        Params.AddIgnoredActor(GetOwner());

        if (World->LineTraceSingleByChannel(WorldHit, Start, MaxEnd, ECC_Visibility, Params) && WorldHit.bBlockingHit)
        {
            BestDist = WorldHit.Distance;
            End = WorldHit.ImpactPoint;
        }
    }

    // Widget hit from the widget interaction (UI precision)
    const FHitResult& Hit = WIC->GetLastHitResult();
    if (Hit.bBlockingHit)
    {
        const float WidgetDist = (Hit.ImpactPoint - Start).Size();
        if (WidgetDist < BestDist)
        {
            BestDist = WidgetDist;
            End = Hit.ImpactPoint;
        }
    }

    const FVector Delta = End - Start;
    const float Length = FMath::Max(1.f, Delta.Size());

    const FVector Mid = Start + Delta * 0.5f;
    const FVector ZAxis = Length > KINDA_SMALL_NUMBER ? Delta / Length : FVector::ForwardVector;

    // Cylinder is oriented along local +Z
    const FRotator Rot = FRotationMatrix::MakeFromZ(ZAxis).Rotator();

    SetActorLocationAndRotation(Mid, Rot);

    // Scale: X/Y control radius, Z controls length
    UStaticMesh* SM = Beam->GetStaticMesh();
    float BaseRadiusCm = 50.f;
    if (SM)
    {
        const FBoxSphereBounds B = SM->GetBounds();
        BaseRadiusCm = FMath::Max(1.f, B.BoxExtent.X); // X ~= radius in cm
    }

    const float ScaleXY = RadiusCm / BaseRadiusCm;
    const float ScaleZ = Length / MeshLengthCm;

    Beam->SetRelativeLocation(FVector::ZeroVector);
    Beam->SetRelativeRotation(FRotator::ZeroRotator);
    Beam->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));
}
