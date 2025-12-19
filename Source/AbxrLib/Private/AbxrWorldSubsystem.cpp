#include "AbxrWorldSubsystem.h"
#include "Abxr.h"
#include "VRPopupLibrary.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"


static void DumpMotionControllers(AActor* Owner)
{
    TArray<UMotionControllerComponent*> MCs;
    Owner->GetComponents(MCs);

    UE_LOG(LogTemp, Warning, TEXT("Found %d MotionControllerComponents on %s"), MCs.Num(), *GetNameSafe(Owner));
    for (auto* MC : MCs)
    {
        UE_LOG(LogTemp, Warning, TEXT("  MC Name=%s MotionSource=%s"),
            *GetNameSafe(MC),
            *MC->MotionSource.ToString());
    }
}

static UMotionControllerComponent* FindMCAny(APawn* Pawn, const TArray<FName>& Sources)
{
    if (!Pawn) return nullptr;

    TArray<UMotionControllerComponent*> MCs;
    Pawn->GetComponents(MCs);

    for (FName S : Sources)
    {
        for (auto* MC : MCs)
        {
            if (MC && MC->MotionSource == S)
                return MC;
        }
    }
    return nullptr;
}

static UWidgetInteractionComponent* FindWICByName(APawn* Pawn, FName Name)
{
    TArray<UWidgetInteractionComponent*> WICs;
    Pawn->GetComponents(WICs);
    for (auto* W : WICs)
    {
        if (W && W->GetFName() == Name) return W;
    }
    return nullptr;
}

static UWidgetInteractionComponent* EnsureWIC(APawn* Pawn, USceneComponent* AttachTo, FName Name, int32 PointerIndex)
{
    if (!Pawn || !AttachTo) return nullptr;

    if (UWidgetInteractionComponent* Existing = FindWICByName(Pawn, Name))
        return Existing;

    UWidgetInteractionComponent* WIC = NewObject<UWidgetInteractionComponent>(Pawn, Name);
    WIC->RegisterComponent();
    WIC->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepRelativeTransform);

    WIC->InteractionSource   = EWidgetInteractionSource::World;
    WIC->InteractionDistance = 1000.f;
    WIC->VirtualUserIndex    = 0;
    WIC->PointerIndex        = PointerIndex;

    // IMPORTANT: makes a visible line/sphere so you know it's alive
    WIC->bShowDebug = true;

    return WIC;
}

void InstallLasers(APawn* Pawn)
{
    if (!Pawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("InstallLasers: Pawn is null"));
        return;
    }

    // Make sure you're doing this on the local player
    if (!Pawn->IsLocallyControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("InstallLasers: Pawn is not locally controlled (%s)"), *GetNameSafe(Pawn));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("InstallLasers on %s (%s)"),
        *GetNameSafe(Pawn), *Pawn->GetClass()->GetPathName());

    DumpMotionControllers(Pawn);

    // Common OpenXR sources to try
    UMotionControllerComponent* LeftMC = FindMCAny(Pawn, {TEXT("Left"), TEXT("LeftAim"), TEXT("LeftGrip")});
    UMotionControllerComponent* RightMC = FindMCAny(Pawn, {TEXT("Right"), TEXT("RightAim"), TEXT("RightGrip")});

    UE_LOG(LogTemp, Warning, TEXT("LeftMC=%s RightMC=%s"), *GetNameSafe(LeftMC), *GetNameSafe(RightMC));

    if (LeftMC)  EnsureWIC(Pawn, LeftMC,  TEXT("LeftWIC"),  0);
    if (RightMC) EnsureWIC(Pawn, RightMC, TEXT("RightWIC"), 1);

    if (!LeftMC && !RightMC)
    {
        UE_LOG(LogTemp, Warning, TEXT("InstallLasers: No motion controllers found on the pawn."));
        UE_LOG(LogTemp, Warning, TEXT("If the template uses ChildActorComponents for hands, we need to search attached child actors too."));
    }
}


void UAbxrWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld()) return; // skip editor/preview worlds you don’t want
	
	UAbxr::SetWorld(World);
	bWorldReady = true;

	if (!World) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;

	UE_LOG(LogTemp, Warning, TEXT("PC=%s Pawn=%s Class=%s"),
		*GetNameSafe(PC),
		*GetNameSafe(Pawn),
		Pawn ? *Pawn->GetClass()->GetPathName() : TEXT("None"));
}

void UAbxrWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	bWorldReady = false;
}

void UAbxrWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	InWorld.GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&InWorld]
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(&InWorld, 0);
		APawn* Pawn = PC ? PC->GetPawn() : nullptr;
		InstallLasers(Pawn);

		UE_LOG(LogTemp, Warning, TEXT("[AfterBeginPlay] PC=%s Pawn=%s Class=%s World=%s"),
			*GetNameSafe(PC),
			*GetNameSafe(Pawn),
			Pawn ? *Pawn->GetClass()->GetPathName() : TEXT("None"),
			*GetNameSafe(&InWorld));
	}));
	
	FTimerHandle Handle;
	InWorld.GetTimerManager().SetTimer(Handle, [&InWorld]
	{
		UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(&InWorld, 1000.f, 0.f);
	}, 0.2f, false);
}
