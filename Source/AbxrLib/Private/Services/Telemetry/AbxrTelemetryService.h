#pragma once
#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"

class UGameInstance;

/**
 * Collects and sends telemetry (FPS, memory, player position, etc.) periodically
 */
class FAbxrTelemetryService : public TSharedFromThis<FAbxrTelemetryService>
{
public:
	explicit FAbxrTelemetryService(UGameInstance* InGameInstance);
	void Start();
	void Stop();

private:
	void CaptureTelemetry() const;
	void CaptureFrameRate() const;
	void CapturePositionData() const;
	FTimerHandle TelemetryTimerHandle;
	FTimerHandle FrameRateTimerHandle;
	FTimerHandle PositionDataTimerHandle;
	
	UWorld* GetWorld() const;
	TWeakObjectPtr<UGameInstance> GameInstance;
	bool bStarted = false;
};
