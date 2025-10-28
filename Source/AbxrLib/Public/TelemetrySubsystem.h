#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TelemetrySubsystem.generated.h"

/**
 * Collects and sends telemetry (FPS, memory, player position, etc.) periodically.
 */
UCLASS()
class ABXRLIB_API UTelemetrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	private:
	void CaptureTelemetry() const;
	FTimerHandle TelemetryTimerHandle;
	float SendInterval = 5.f;
};
