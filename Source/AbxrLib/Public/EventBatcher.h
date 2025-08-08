#pragma once

#include "CoreMinimal.h"
#include "EventBatcher.generated.h"

class EventBatcher
{
public:
	static void Add(FString Name, const TMap<FString, FString>& Meta);
	static void Send();
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
