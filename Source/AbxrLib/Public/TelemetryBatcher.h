#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
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
	static void Add(FString Name, const TMap<FString, FString>& Meta);
	static void Send();
	static void Start();
	static void Stop();

private:
	static bool Tick(float Dt);
	
	static FCriticalSection Mutex;
	static TArray<FAbxrTelemetryPayload> Payloads;
	static int64 LastCallTime;
	static bool bStarted;
	static double NextAt;
	static FTSTicker::FDelegateHandle Ticker;
	static constexpr double MaxCallFrequencySeconds = 1;
};
