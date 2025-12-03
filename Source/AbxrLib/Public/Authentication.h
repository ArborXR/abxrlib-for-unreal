#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include <thread>
#include <atomic>
#include "Authentication.generated.h"


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

	UPROPERTY() FString appId;
	UPROPERTY() FString orgId;
	UPROPERTY() FString authSecret;
	UPROPERTY() FString deviceId;
	UPROPERTY() FString userId;
	UPROPERTY() TArray<FString> tags;
	UPROPERTY() FString sessionId;
	UPROPERTY() FString partner;
	UPROPERTY() FString ipAddress;
	UPROPERTY() FString deviceModel;
	UPROPERTY() TMap<FString, FString> geolocation;
	UPROPERTY() FString osVersion;
	UPROPERTY() FString xrdmVersion;
	UPROPERTY() FString appVersion;
	UPROPERTY() FString unrealVersion;
	UPROPERTY() FString abxrLibVersion;
	UPROPERTY() TMap<FString, FString> authMechanism;
};

USTRUCT()
struct FAuthMechanism
{
	GENERATED_BODY()

	UPROPERTY() FString type;
	UPROPERTY() FString prompt;
	UPROPERTY() FString domain;
};

USTRUCT()
struct FConfigPayload
{
	GENERATED_BODY()

	UPROPERTY() FAuthMechanism authMechanism;
	UPROPERTY() FString frameRateCapturePeriod;
	UPROPERTY() FString telemetryCapturePeriod;
	UPROPERTY() FString restUrl;
	UPROPERTY() FString sendRetriesOnFailure;
	UPROPERTY() FString sendRetryInterval;
	UPROPERTY() FString sendNextBatchWait;
	UPROPERTY() FString stragglerTimeout;
	UPROPERTY() FString dataEntriesPerSendAttempt;
	UPROPERTY() FString storageEntriesPerSendAttempt;
	UPROPERTY() FString pruneSentItemsOlderThan;
	UPROPERTY() FString maximumCachedItems;
	UPROPERTY() FString retainLocalAfterSent;
	UPROPERTY() FString positionCapturePeriod;
};

class Authentication
{
public:
	static void Authenticate();
	static void PollForReAuth();
	
	static bool Authenticated()
	{
		const FDateTime Now = FDateTime::UtcNow();
		return !AuthToken.IsEmpty() && !ApiSecret.IsEmpty() && Now.ToUnixTimestamp() <= TokenExpiry && NeedKeyboardAuth == false;
	}
	
	static void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json);
	static void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request)
	{
		SetAuthHeaders(Request, TEXT(""));
	}

	static void KeyboardAuthenticate();
	static void KeyboardAuthenticate(const FString& KeyboardInput);

private:
	static TMap<FString, FString> CreateAuthMechanismDict();
	static void ClearAuthenticationState();
	static void AuthRequest(TFunction<void(bool)> OnComplete);
	static void GetConfiguration(TFunction<void(bool)> OnComplete);
	static void SetConfigFromPayload(const FConfigPayload& Payload);
	static void GetConfigData();
	static void GetArborData();

	static std::thread ReAuthThread;
	static std::atomic<bool> bShouldStop;
    
	static void ReAuthThreadFunction();
	static void CheckReauthentication();

	static FString AuthToken;
	static FString ApiSecret;
	static FString SessionId;
	static int TokenExpiry;
	static FAuthMechanism AuthMechanism;
	static int FailedAuthAttempts;
	static std::optional<bool> NeedKeyboardAuth;

	static FString OrgId;
	static FString DeviceId;
	static FString AuthSecret;
	static FString AppId;
	static FString Partner;
	static FString DeviceModel;
	static TArray<FString> DeviceTags;
	static FString XrdmVersion;
	static FString IpAddress;
};
