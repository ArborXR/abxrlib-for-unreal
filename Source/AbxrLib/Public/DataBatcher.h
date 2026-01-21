#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Types/AbxrTypes.h"
#include "HAL/CriticalSection.h"

class DataBatcher
{
public:
	static void AddEvent(const FString& Name, const TMap<FString, FString>& Meta);
	static void AddTelemetry(const FString& Name, const TMap<FString, FString>& Meta);
	static void AddLog(const FString& Level, const FString& Text, const TMap<FString, FString>& Meta);

	static void Start();
	static void Stop();
	static void Send();

private:
	static bool Tick(float DeltaTime);

	static FCriticalSection Mutex;
	static TArray<FAbxrEventPayload> EventPayloads;
	static TArray<FAbxrTelemetryPayload> TelemetryPayloads;
	static TArray<FAbxrLogPayload> LogPayloads;

	static int64 LastCallTime;
	static bool bStarted;
	static double NextAt;
	static FTSTicker::FDelegateHandle Ticker;
};
