#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "TelemetryBatcher.generated.h"

USTRUCT()
struct FAbxrTelemetryPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FString preciseTimestamp;

	UPROPERTY()
	FString name;

	UPROPERTY()
	TMap<FString, FString> meta;
};

USTRUCT()
struct FAbxrTelemetryPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAbxrTelemetryPayload> data;
};

class TelemetryBatcher
{
public:
	static void Init(const UWorld* World);
	static void Add(FString Name, const TMap<FString, FString>& Meta);
	static void Send();

private:
	static FTimerHandle TimerHandle;
	static int Timer;
	static FCriticalSection Mutex;
	static TArray<FAbxrTelemetryPayload> Payloads;
	static int64 LastCallTime;
	static constexpr double MaxCallFrequencySeconds = 1;
};
