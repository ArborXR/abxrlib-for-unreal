#include "AbxrTelemetryService.h"
#include "AbxrLibAPI.h"
#include "Services/Config/AbxrSettings.h"
#include "Engine/Engine.h"
#include "Misc/App.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Types/AbxrLog.h"
#if PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
#endif

FAbxrTelemetryService::FAbxrTelemetryService(UGameInstance* InGameInstance) : GameInstance(InGameInstance) { }

UWorld* FAbxrTelemetryService::GetWorld() const
{
    return GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
}

void FAbxrTelemetryService::Start()
{
    if (bStarted) return;

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("TelemetryService Start failed: no valid world"));
        return;
    }

    bStarted = true;

    const UAbxrSettings* Settings = GetDefault<UAbxrSettings>();
    if (Settings->EnableAutomaticTelemetry)
    {
        World->GetTimerManager().SetTimer(
            TelemetryTimerHandle,
            FTimerDelegate::CreateSP(AsShared(), &FAbxrTelemetryService::CaptureTelemetry),
            Settings->TelemetryTrackingPeriodSeconds,
            true
        );
    }

    World->GetTimerManager().SetTimer(
        FrameRateTimerHandle,
        FTimerDelegate::CreateSP(AsShared(), &FAbxrTelemetryService::CaptureFrameRate),
        Settings->FrameRateTrackingPeriodSeconds,
        true
    );

    if (Settings->HeadsetControllerTracking)
    {
        World->GetTimerManager().SetTimer(
            PositionDataTimerHandle,
            FTimerDelegate::CreateSP(AsShared(), &FAbxrTelemetryService::CapturePositionData),
            Settings->PositionCapturePeriodSeconds,
            true
        );
    }
}

void FAbxrTelemetryService::Stop()
{
    if (!bStarted) return;

    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TelemetryTimerHandle);
        World->GetTimerManager().ClearTimer(FrameRateTimerHandle);
        World->GetTimerManager().ClearTimer(PositionDataTimerHandle);
    }

    bStarted = false;
}

void FAbxrTelemetryService::CaptureFrameRate() const
{
    const float FPS = FApp::GetDeltaTime() > 0.f ? 1.f / FApp::GetDeltaTime() : 0.f;
    TMap<FString, FString> Meta;
    Meta.Add(TEXT("Per Second"), LexToString(FPS));
    Abxr::Telemetry(TEXT("Frame Rate"), Meta);
}

void FAbxrTelemetryService::CaptureTelemetry() const
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

void FAbxrTelemetryService::CapturePositionData() const
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
