#include "TelemetrySubsystem.h"
#include "Abxr.h"
#include "AbxrLibConfiguration.h"
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
            GetDefault<UAbxrLibConfiguration>()->TelemetryTrackingPeriodSeconds,
            true // loop
        );

        World->GetTimerManager().SetTimer(
            FrameRateTimerHandle,
            this,
            &UTelemetrySubsystem::CaptureFrameRate,
            GetDefault<UAbxrLibConfiguration>()->FrameRateTrackingPeriodSeconds,
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

void UTelemetrySubsystem::CaptureFrameRate() const
{
    const float FPS = FApp::GetDeltaTime() > 0.f ? 1.f / FApp::GetDeltaTime() : 0.f;
    TMap<FString, FString> Meta;
    Meta[TEXT("Per Second")] = FString::FromInt(FPS);
    UAbxr::Telemetry(TEXT("Frame Rate"), Meta);
}

void UTelemetrySubsystem::CaptureTelemetry() const
{
    const FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    TMap<FString, FString> Meta;
    Meta[TEXT("Used Physical")] = FString::FromInt(MemStats.UsedPhysical / 1024.0 / 1024.0) + TEXT(" MB");
    UAbxr::Telemetry(TEXT("Memory"), Meta);
#if PLATFORM_ANDROID
    Meta.Empty();
    Meta[TEXT("Percentage")] = FString::FromInt(FAndroidMisc::GetBatteryState().Level) + TEXT("%");
    Meta[TEXT("Temperature")] = FString::FromInt(FAndroidMisc::GetBatteryState().Temperature) + TEXT(" C");
    UAbxr::Telemetry(TEXT("Battery"), Meta);
#endif
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

    Meta.Empty();
    Meta[TEXT("x")] = FString::SanitizeFloat(PlayerLocation.X);
    Meta[TEXT("y")] = FString::SanitizeFloat(PlayerLocation.Y);
    Meta[TEXT("z")] = FString::SanitizeFloat(PlayerLocation.Z);
    UAbxr::Telemetry(TEXT("Player Location"), Meta);

    Meta.Empty();
    Meta[TEXT("Yaw")] = FString::SanitizeFloat(PlayerRotation.Yaw);
    Meta[TEXT("Pitch")] = FString::SanitizeFloat(PlayerRotation.Pitch);
    Meta[TEXT("Roll")] = FString::SanitizeFloat(PlayerRotation.Roll);
    UAbxr::Telemetry(TEXT("Player Rotation"), Meta);
}
