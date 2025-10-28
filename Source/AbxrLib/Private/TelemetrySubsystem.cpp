#include "TelemetrySubsystem.h"
#include "Engine/Engine.h"
#include "Misc/App.h"
#include "Misc/EngineVersion.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "TimerManager.h"
#if PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
#endif

void UTelemetrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            TelemetryTimerHandle,
            this,
            &UTelemetrySubsystem::CaptureTelemetry,
            SendInterval,
            true // loop
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Unable to initialize TelemetrySubsystem"));
    }
}

void UTelemetrySubsystem::Deinitialize()
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TelemetryTimerHandle);
    }
    
    Super::Deinitialize();
}

void UTelemetrySubsystem::CaptureTelemetry() const
{
    const float FPS = FApp::GetDeltaTime() > 0.f ? 1.f / FApp::GetDeltaTime() : 0.f;
    const FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    
    FVector PlayerLocation = FVector::ZeroVector;
    FRotator PlayerRotation = FRotator::ZeroRotator;

    if (const UWorld* World = GetWorld())
    {
        if (const APlayerController* PC = World->GetFirstPlayerController())
        {
            if (const APawn* Pawn = PC->GetPawn())
            {
                PlayerLocation = Pawn->GetActorLocation();
                PlayerRotation = Pawn->GetActorRotation();
            }
        }
    }

    //const FString MapName = GetWorld() ? GetWorld()->GetMapName() : TEXT("Unknown");

    UE_LOG(LogTemp, Error, TEXT("AbxrLib - FPS - %s"), *FString::FromInt(FPS));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - MemoryMB - %s"), *FString::FromInt(MemStats.UsedPhysical / 1024.0 / 1024.0));
#if PLATFORM_ANDROID
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Battery Level - %s"), *FString::FromInt(FAndroidMisc::GetBatteryState().Level));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Temperature - %sC"), *FString::SanitizeFloat(FAndroidMisc::GetBatteryState().Temperature));
#endif
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - X - %s"), *FString::SanitizeFloat(PlayerLocation.X));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Y - %s"), *FString::SanitizeFloat(PlayerLocation.Y));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Z - %s"), *FString::SanitizeFloat(PlayerLocation.Z));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Yaw - %s"), *FString::SanitizeFloat(PlayerRotation.Yaw));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Pitch - %s"), *FString::SanitizeFloat(PlayerRotation.Pitch));
    UE_LOG(LogTemp, Error, TEXT("AbxrLib - Roll - %s"), *FString::SanitizeFloat(PlayerRotation.Roll));
}
