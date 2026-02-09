#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/AbxrTypes.h"

class FAbxrAuthService : public TSharedFromThis<FAbxrAuthService>
{
public:
	explicit FAbxrAuthService(const FAbxrAuthCallbacks& callbacks);
	~FAbxrAuthService();
	
	void Authenticate();
	bool Authenticated() const { return bAuthenticated; }
	FAbxrAuthResponse GetAuthResponse() { return ResponseData; }
	void SetSessionId(const FString& sessionId) { Payload.SessionId = sessionId; }
	void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json) const;
	void KeyboardAuthenticate(const FString& KeyboardInput);
	void StopReAuthPolling();

private:
	static bool ShouldRetry(bool bOk, const FHttpResponsePtr& Response);
	void ScheduleRetry(TFunction<void()> Fn);
	void CancelRetryTimer();

	TMap<FString, FString> CreateAuthMechanismDict() const;
	void ClearAuthenticationState();
	void AuthRequest(TFunction<void(bool)> OnComplete);
	bool ParseAuthResponse(const FString& Body, const bool Handoff);
	void GetConfiguration(TFunction<void(bool)> OnComplete);
	static void SetConfigFromPayload(const FAbxrConfigPayload& Payload);
	void SetAuthHeaders(const TSharedRef<IHttpRequest>& Request) const { SetAuthHeaders(Request, TEXT("")); }
	void GetConfigData();
	void GetArborData();
	void AuthSucceeded();
	void KeyboardAuthenticate();
	
	bool CheckAuthHandoff();
	static FString GetCommandLineArg(const FString& Key);
	FString GetAndroidIntentParam(const FString& Key) const;
	bool SessionUsedAuthHandoff;
	
	bool ReAuthTick();
	void StartReAuthPolling();
	
	FAbxrAuthCallbacks Callbacks;
	FThreadSafeBool bStopping{false};
	FThreadSafeBool bAttemptActive{false};
	FHttpRequestPtr ActiveRequest;

	bool bAuthenticated;
	FAbxrAuthResponse ResponseData;
	
	FAbxrAuthPayload Payload;
	int TokenExpiry;
	FAbxrAuthMechanism AuthMechanism;
	int FailedAuthAttempts;
	
	FTSTicker::FDelegateHandle ReAuthTickHandle;
	double NextReAuthCheckAtSeconds = 0.0;
	
	FTSTicker::FDelegateHandle RetryTickHandle;
	static constexpr int RetryMaxAttempts = 3;
	static constexpr int RetryDelaySeconds = 1;

	static constexpr double ReAuthPollSeconds = 30.0;
	static constexpr int32 ReAuthThresholdSeconds = 120;
};
