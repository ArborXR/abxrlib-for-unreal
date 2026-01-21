#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/AbxrTypes.h"
#include <thread>
#include <atomic>

class Authentication
{
public:
	static void Authenticate();
	static void PollForReAuth();
	static void SetSessionId(const FString& sessionId) { SessionId = sessionId; }
	static FAbxrAuthResponse GetAuthResponse() { return ResponseData; }
	
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
	static void SetConfigFromPayload(const FAbxrConfigPayload& Payload);
	static void GetConfigData();
	static void GetArborData();

	static std::thread ReAuthThread;
	static std::atomic<bool> bShouldStop;
    
	static void ReAuthThreadFunction();
	static void CheckReauthentication();

	static FAbxrAuthResponse ResponseData;
	
	static FString SessionId;
	static int TokenExpiry;
	static FAbxrAuthMechanism AuthMechanism;
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
