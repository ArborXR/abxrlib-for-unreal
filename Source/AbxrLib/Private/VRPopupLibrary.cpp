// VRPopupLibrary.cpp

#include "VRPopupLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AActor* UVRPopupLibrary::SpawnPopupButtonInFrontOfPlayer(
    UObject* WorldContextObject,
    float Distance,
    float VerticalOffset)
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

    // Hard-coded asset path to BP_VRPopupActor
    // Make sure this matches your "Copy Reference", with _C at the end
    /*const TCHAR* PopupActorPath = TEXT("Blueprint'/Game/VRPopup/BP_VRPopupActor.BP_VRPopupActor_C'");
    
    UClass* PopupActorClass = LoadClass<AActor>(nullptr, PopupActorPath);
    if (!PopupActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: Failed to load class from path: %s"), PopupActorPath);
        return nullptr;
    }*/

    TSoftClassPtr<AActor> PopupActorPtr(FSoftObjectPath(TEXT("/AbxrLib/BP_VRPopupActor.BP_VRPopupActor_C")));
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
    FVector SpawnLocation = ViewLocation + Forward * Distance;
    SpawnLocation.Z += VerticalOffset;

    // Make it face the player, but keep it level if you want
    FRotator SpawnRotation = ViewRotation;
    SpawnRotation.Pitch = 0.f;
    SpawnRotation.Roll = 0.f;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Spawned = World->SpawnActor<AActor>(
        PopupActorClass,
        SpawnLocation,
        SpawnRotation,
        Params
    );

    if (!Spawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPopupButtonInFrontOfPlayer: Failed to spawn popup actor"));
    }

    return Spawned;
}
