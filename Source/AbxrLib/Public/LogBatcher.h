#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "LogBatcher.generated.h"

USTRUCT()
struct FAbxrLogPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FString preciseTimestamp;

	UPROPERTY()
	FString logLevel;

	UPROPERTY()
	FString text;

	UPROPERTY()
	TMap<FString, FString> meta;
};

USTRUCT()
struct FAbxrLogPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAbxrLogPayload> data;
};

class LogBatcher
{
public:
	static void Add(FString LogLevel, FString Text, const TMap<FString, FString>& Meta);
	static void Send();
	static void Start();
	static void Stop();

private:
	static bool Tick(float Dt);
	
	static FCriticalSection Mutex;
	static TArray<FAbxrLogPayload> Payloads;
	static int64 LastCallTime;
	static bool bStarted;
	static double NextAt;
	static FTSTicker::FDelegateHandle Ticker;
	static constexpr double MaxCallFrequencySeconds = 1;
};
