#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Authentication.generated.h"

class Authentication
{
public:
	static void Authenticate();
	
	static void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json);
	static void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request)
	{
		SetAuthHeaders(Request, TEXT(""));
	}

private:
	static TMap<FString, FString> CreateAuthMechanismDict();

	static FString AuthToken;
	static FString ApiSecret;
	static FString SessionId;
	static int TokenExpiry;
};

UENUM(BlueprintType)
enum class EPartner : uint8
{
	None,
	ArborXR
};

USTRUCT()
struct FAuthPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FString appId;

	UPROPERTY()
	FString orgId;

	UPROPERTY()
	FString authSecret;

	UPROPERTY()
	FString deviceId;

	UPROPERTY()
	FString userId;

	UPROPERTY()
	TArray<FString> tags;
	
	UPROPERTY()
	FString sessionId;

	UPROPERTY()
	FString partner;

	UPROPERTY()
	FString ipAddress;

	UPROPERTY()
	FString deviceModel;

	UPROPERTY()
	TMap<FString, FString> geolocation;

	UPROPERTY()
	FString osVersion;

	UPROPERTY()
	FString xrdmVersion;

	UPROPERTY()
	FString appVersion;

	UPROPERTY()
	FString unrealVersion;
	
	UPROPERTY()
	FString abxrLibVersion;

	UPROPERTY()
	TMap<FString, FString> authMechanism;
};

USTRUCT()
struct FAuthMechanism
{
	GENERATED_BODY()

	UPROPERTY()
	FString type;

	UPROPERTY()
	FString prompt;

	UPROPERTY()
	FString domain;
};

USTRUCT()
struct FConfigPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FAuthMechanism authMechanism;

	UPROPERTY()
	FString frameRateCapturePeriod;

	UPROPERTY()
	FString telemetryCapturePeriod;

	UPROPERTY()
	FString restUrl;

	UPROPERTY()
	FString sendRetriesOnFailure;

	UPROPERTY()
	FString sendTreyInterval;
	
	UPROPERTY()
	FString sendNextBatchWait;

	UPROPERTY()
	FString stragglerTimeout;

	UPROPERTY()
	FString eventsPerSendAttempt;

	UPROPERTY()
	FString logsPerSendAttempt;

	UPROPERTY()
	FString telemetryEntriesPerSendAttempt;

	UPROPERTY()
	FString storageEntriesPerSendAttempt;

	UPROPERTY()
	FString pruneSentItemsOlderThan;

	UPROPERTY()
	FString maximumCachedItems;
	
	UPROPERTY()
	FString retainLocalAfterSent;

	UPROPERTY()
	FString positionCapturePeriod;
};
