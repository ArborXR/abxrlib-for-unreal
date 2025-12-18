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

	UPROPERTY() FString AppId;
	UPROPERTY() FString OrgId;
	UPROPERTY() FString AuthSecret;
	UPROPERTY() FString DeviceId;
	UPROPERTY() FString UserId;
	UPROPERTY() TArray<FString> Tags;
	UPROPERTY() FString SessionId;
	UPROPERTY() FString Partner;
	UPROPERTY() FString IpAddress;
	UPROPERTY() FString DeviceModel;
	UPROPERTY() TMap<FString, FString> Geolocation;
	UPROPERTY() FString OsVersion;
	UPROPERTY() FString XrdmVersion;
	UPROPERTY() FString AppVersion;
	UPROPERTY() FString UnrealVersion;
	UPROPERTY() FString AbxrLibVersion;
	UPROPERTY() TMap<FString, FString> AuthMechanism;
};

USTRUCT()
struct FModuleData
{
	GENERATED_BODY()
	
	UPROPERTY() FString Id;
	UPROPERTY() FString Name;
	UPROPERTY() FString Target;
	UPROPERTY() int Order = 0;
};

USTRUCT()
struct FAuthResponse
{
	GENERATED_BODY()

	UPROPERTY() FString Token;
	UPROPERTY() FString Secret;
	UPROPERTY() TMap<FString, FString> UserData;
	UPROPERTY() FString UserId;
	UPROPERTY() FString AppId;
	UPROPERTY() FString PackageName;
	UPROPERTY() TArray<FModuleData> Modules;
};

USTRUCT()
struct FAuthMechanism
{
	GENERATED_BODY()

	UPROPERTY() FString Type;
	UPROPERTY() FString Prompt;
	UPROPERTY() FString Domain;
};

USTRUCT()
struct FConfigPayload
{
	GENERATED_BODY()

	UPROPERTY() FAuthMechanism AuthMechanism;
	UPROPERTY() FString FrameRateCapturePeriod;
	UPROPERTY() FString TelemetryCapturePeriod;
	UPROPERTY() FString RestUrl;
	UPROPERTY() FString SendRetriesOnFailure;
	UPROPERTY() FString SendRetryInterval;
	UPROPERTY() FString SendNextBatchWait;
	UPROPERTY() FString StragglerTimeout;
	UPROPERTY() FString DataEntriesPerSendAttempt;
	UPROPERTY() FString StorageEntriesPerSendAttempt;
	UPROPERTY() FString PruneSentItemsOlderThan;
	UPROPERTY() FString MaximumCachedItems;
	UPROPERTY() FString RetainLocalAfterSent;
	UPROPERTY() FString PositionCapturePeriod;
};

class Authentication
{
public:
	static void Authenticate();
	static void PollForReAuth();
	static void SetSessionId(const FString& sessionId) { SessionId = sessionId; }
	static FAuthResponse GetAuthResponse() { return ResponseData; }
	
	static bool Authenticated()
	{
		const FDateTime Now = FDateTime::UtcNow();
		return !ResponseData.Token.IsEmpty() && !ResponseData.Secret.IsEmpty() && Now.ToUnixTimestamp() <= TokenExpiry && NeedKeyboardAuth == false;
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

	static FAuthResponse ResponseData;
	
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
