#pragma once
#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Types/AbxrTypes.h"
#include "Services/Auth/AbxrAuthService.h"
#include "HAL/CriticalSection.h"

class FAbxrDataService
{
public:
	explicit FAbxrDataService(class FAbxrAuthService& AuthService) : AuthService(AuthService), LastCallTime(0), bStarted(false), NextAt(0) { }

	void AddEvent(const FString& Name, const TMap<FString, FString>& Meta);
	void AddTelemetry(const FString& Name, const TMap<FString, FString>& Meta);
	void AddLog(const FString& Level, const FString& Text, const TMap<FString, FString>& Meta);

	void Start();
	void Stop();
	void Send();

private:
	bool Tick(float DeltaTime);
	
	FAbxrAuthService& AuthService;

	FCriticalSection Mutex;
	TArray<FAbxrEventPayload> EventPayloads;
	TArray<FAbxrTelemetryPayload> TelemetryPayloads;
	TArray<FAbxrLogPayload> LogPayloads;

	int64 LastCallTime;
	bool bStarted;
	double NextAt;
	FTSTicker::FDelegateHandle Ticker;
};
