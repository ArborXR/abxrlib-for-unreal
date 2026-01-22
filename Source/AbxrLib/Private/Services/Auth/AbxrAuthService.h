#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/AbxrTypes.h"
#include <thread>
#include <atomic>

class FAbxrAuthService : public TSharedFromThis<FAbxrAuthService>
{
public:
	FAbxrAuthService() : bShouldStop(false), TokenExpiry(0), FailedAuthAttempts(0), Partner(TEXT("none")) {}
	
	void Authenticate();
	void StartReAuthPolling();
	void StopReAuthPolling();
	void SetSessionId(const FString& sessionId) { SessionId = sessionId; }
	FAbxrAuthResponse GetAuthResponse() { return ResponseData; }
	
	bool Authenticated() const
	{
		const FDateTime Now = FDateTime::UtcNow();
		return !ResponseData.Token.IsEmpty() && !ResponseData.Secret.IsEmpty() && Now.ToUnixTimestamp() <= TokenExpiry && NeedKeyboardAuth == false;
	}
	
	void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json);
	void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request)
	{
		SetAuthHeaders(Request, TEXT(""));
	}

	void KeyboardAuthenticate();
	void KeyboardAuthenticate(const FString& KeyboardInput);

private:
	TMap<FString, FString> CreateAuthMechanismDict();
	void ClearAuthenticationState();
	void AuthRequest(TFunction<void(bool)> OnComplete);
	void GetConfiguration(TFunction<void(bool)> OnComplete);
	void SetConfigFromPayload(const FAbxrConfigPayload& Payload);
	void GetConfigData();
	void GetArborData();

	std::thread ReAuthThread;
	std::atomic<bool> bShouldStop;
    
	bool ReAuthTick();
	void CheckReauthentication();

	FAbxrAuthResponse ResponseData;
	
	FString SessionId;
	int TokenExpiry;
	FAbxrAuthMechanism AuthMechanism;
	int FailedAuthAttempts;
	std::optional<bool> NeedKeyboardAuth;

	FString OrgId;
	FString DeviceId;
	FString AuthSecret;
	FString AppId;
	FString Partner;
	FString DeviceModel;
	TArray<FString> DeviceTags;
	FString XrdmVersion;
	FString IpAddress;
	
	FTSTicker::FDelegateHandle ReauthTickHandle;
	bool bReauthInFlight = false;
	double NextReauthCheckAtSeconds = 0.0;

	static constexpr double ReauthPollSeconds = 30.0;
	static constexpr int32 ReauthThresholdSeconds = 120;
};
