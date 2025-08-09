#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"
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
	static void Init(const UWorld* World);
	static void Add(FString LogLevel, FString Text, const TMap<FString, FString>& Meta);
	static void Send();

private:
	static FTimerHandle TimerHandle;
	static int Timer;
	static FCriticalSection Mutex;
	static TArray<FAbxrLogPayload> Payloads;
	static int64 LastCallTime;
	static constexpr double MaxCallFrequencySeconds = 1;
};
