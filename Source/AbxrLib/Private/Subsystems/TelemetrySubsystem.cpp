#include "TelemetrySubsystem.h"

#include "AbxrLibAPI.h"
#include "Services/Config/AbxrSettings.h"
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
    Collection.InitializeDependency<UAbxrSubsystem>();
    Super::Initialize(Collection);

    if (const UWorld* World = GetWorld())
    {
        if (GetDefault<UAbxrSettings>()->EnableAutomaticTelemetry)
        {
            World->GetTimerManager().SetTimer(
                TelemetryTimerHandle,
                this,
                &UTelemetrySubsystem::CaptureTelemetry,
                GetDefault<UAbxrSettings>()->TelemetryTrackingPeriodSeconds,
                true // loop
            );
        }

        World->GetTimerManager().SetTimer(
            FrameRateTimerHandle,
            this,
            &UTelemetrySubsystem::CaptureFrameRate,
            GetDefault<UAbxrSettings>()->FrameRateTrackingPeriodSeconds,
            true // loop
        );

        if (GetDefault<UAbxrSettings>()->HeadsetControllerTracking)
        {
            World->GetTimerManager().SetTimer(
                PositionDataTimerHandle,
                this,
                &UTelemetrySubsystem::CapturePositionData,
                GetDefault<UAbxrSettings>()->PositionCapturePeriodSeconds,
                true // loop
            );
        }
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

void UTelemetrySubsystem::CaptureFrameRate()
{
    const float FPS = FApp::GetDeltaTime() > 0.f ? 1.f / FApp::GetDeltaTime() : 0.f;
    TMap<FString, FString> Meta;
    Meta.Add(TEXT("Per Second"), LexToString(FPS));
    Abxr::Telemetry(TEXT("Frame Rate"), Meta);
}

void UTelemetrySubsystem::CaptureTelemetry() const
{
    const FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    TMap<FString, FString> Meta;
    Meta.Add(TEXT("Used Physical"), FString::FromInt(MemStats.UsedPhysical / 1024.0 / 1024.0) + TEXT(" MB"));
    Abxr::Telemetry(TEXT("Memory"), Meta);
#if PLATFORM_ANDROID
    Meta.Empty();
    Meta.Add(TEXT("Percentage"), FString::FromInt(FAndroidMisc::GetBatteryState().Level) + TEXT("%"));
    Meta.Add(TEXT("Temperature"), FString::FromInt(FAndroidMisc::GetBatteryState().Temperature) + TEXT(" C"));
    Abxr::Telemetry(TEXT("Battery"), Meta);
#endif
}

void UTelemetrySubsystem::CapturePositionData() const
{
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

    TMap<FString, FString> Meta;
    Meta.Add(TEXT("x"), LexToString(PlayerLocation.X));
    Meta.Add(TEXT("y"), LexToString(PlayerLocation.Y));
    Meta.Add(TEXT("z"), LexToString(PlayerLocation.Z));
    Abxr::Telemetry(TEXT("Player Location"), Meta);

    Meta.Empty();
    Meta.Add(TEXT("Yaw"), LexToString(PlayerRotation.Yaw));
    Meta.Add(TEXT("Pitch"), LexToString(PlayerRotation.Pitch));
    Meta.Add(TEXT("Roll"), LexToString(PlayerRotation.Roll));
    Abxr::Telemetry(TEXT("Player Rotation"), Meta);
}
