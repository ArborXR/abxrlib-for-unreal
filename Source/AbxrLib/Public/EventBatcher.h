#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "EventBatcher.generated.h"

class EventBatcher
{
public:
	static void Init(UWorld* World);
	static void Add(FString Name, const TMap<FString, FString>& Meta);
	static void Send();

private:
	FTimerHandle TimerHandle;
};

USTRUCT()
struct FPayload
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
struct FPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPayload> data;
};
