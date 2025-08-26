#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "EventBatcher.generated.h"

USTRUCT()
struct FAbxrEventPayload
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
struct FAbxrEventPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAbxrEventPayload> data;
};

class EventBatcher
{
public:
	static void Add(FString Name, const TMap<FString, FString>& Meta);
	static void Send();
	static void Start();
	static void Stop();

private:
	static bool Tick(float Dt);
	
	static FCriticalSection Mutex;
	static TArray<FAbxrEventPayload> Payloads;
	static int64 LastCallTime;
	static bool bStarted;
	static double NextAt;
	static FTSTicker::FDelegateHandle Ticker;
	static constexpr double MaxCallFrequencySeconds = 1;
};
