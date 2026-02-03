#include "AbxrAuthService.h"
#include "AbxrLibAPI.h"
#include "Services/Config/AbxrSettings.h"
#include "Util/AbxrUtil.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Services/Platform/XRDM/XRDMService.h"
#include "Interfaces/IHttpResponse.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "HAL/PlatformMisc.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Async/Async.h"
#include "Types/AbxrLog.h"

bool FAbxrAuthService::ShouldRetry(const bool bOk, const FHttpResponsePtr& Response)
{
	// Transport failure or no response: retry
	if (!bOk || !Response.IsValid()) return true;

	const int32 Code = Response->GetResponseCode();
	if (Code == 408 || Code == 429) return true;
	if (Code >= 500 && Code <= 599) return true;

	// 4xx (except 408/429) are treated as non-retryable (bad creds / bad request)
	return false;
}

void FAbxrAuthService::CancelRetryTimer()
{
	if (RetryTickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(RetryTickHandle);
		RetryTickHandle.Reset();
	}
}

void FAbxrAuthService::ScheduleRetry(TFunction<void()> Fn)
{
	CancelRetryTimer();
	if (bStopping || !bAttemptActive) return;

	RetryTickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([AuthPtr = AsWeak(), Fn = MoveTemp(Fn)](float) mutable
		{
			const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
			if (!Self || Self->bStopping || !Self->bAttemptActive) return false;
			Fn();
			return false; // one-shot
		}),
		static_cast<float>(RetryDelaySeconds)
	);
}

FAbxrAuthService::FAbxrAuthService(const FAbxrAuthCallbacks& callbacks) : bAuthenticated(false), TokenExpiry(0), FailedAuthAttempts(0)
{
	Callbacks = callbacks;
	FString HMDName = TEXT("None");
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();
	}
	
	Payload = FAbxrAuthPayload();
	Payload.Partner = TEXT("none");
	Payload.DeviceId = FGuid::NewGuid().ToString();  // MDM will override if available
	Payload.Tags = { };
	Payload.IpAddress = TEXT("");
	Payload.DeviceModel = HMDName;
	Payload.Geolocation = TMap<FString, FString>();
	Payload.OsVersion = FPlatformMisc::GetOSVersion();
	Payload.XrdmVersion = TEXT("1.0.0");
	Payload.AppVersion = TEXT("1.0.0");
	Payload.UnrealVersion = FString::Printf(TEXT("%d.%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);
	Payload.AbxrLibVersion = IPluginManager::Get().FindPlugin(TEXT("AbxrLib"))->GetDescriptor().VersionName;
}

FAbxrAuthService::~FAbxrAuthService()
{
	bStopping = true;
	CancelRetryTimer();
	StopReAuthPolling();
	if (ActiveRequest.IsValid()) ActiveRequest->CancelRequest();
	bAttemptActive = false;
}

void FAbxrAuthService::Authenticate()
{
	if (bStopping || bAttemptActive) return;
	bAttemptActive = true;
	StopReAuthPolling();
	ClearAuthenticationState();
	if (!GetDefault<UAbxrSettings>()->IsValid())
	{
		bAttemptActive = false;
		Callbacks.OnFailed(TEXT("Abxr settings are invalid"));
		return;
	}

	GetConfigData();
	
	auto KickAuthChain = [AuthPtr = AsWeak()](bool bConnected)
	{
		// Always marshal to the game thread; during shutdown this avoids cross-thread races.
		AsyncTask(ENamedThreads::GameThread, [AuthPtr, bConnected]
		{
			const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
			if (!Self || Self->bStopping || !Self->bAttemptActive) return;
#if PLATFORM_ANDROID
			if (bConnected) Self->GetArborData();
#endif
			Self->AuthRequest([AuthPtr](const bool bSuccess)
			{
				const TSharedPtr<FAbxrAuthService> Self2 = AuthPtr.Pin();
				if (!Self2 || Self2->bStopping || !Self2->bAttemptActive) return;
				if (bSuccess)
				{
					Self2->GetConfiguration([AuthPtr](const bool)
					{
						const TSharedPtr<FAbxrAuthService> Self3 = AuthPtr.Pin();
						if (!Self3 || Self3->bStopping || !Self3->bAttemptActive) return;
						if (!Self3->AuthMechanism.Prompt.IsEmpty())
						{
							Self3->KeyboardAuthenticate();
						}
						else
						{
							Self3->AuthSucceeded();
						}
					});

					Self2->StartReAuthPolling();
				}
				else
				{
					Self2->bAttemptActive = false;
					Self2->Callbacks.OnFailed(TEXT("Initial authentication request failed"));
				}
			});
		});
	};

#if PLATFORM_ANDROID
	if (UXRDMService* XRDM = UXRDMService::GetInstance())
	{
		const auto Promise = XRDM->WaitForConnection();
		Promise->GetFuture().Next([KickAuthChain](const bool bConnected)
		{
			KickAuthChain(bConnected);
		});
	}
	else
	{
		KickAuthChain(false);
	}
#else
	KickAuthChain(false);
#endif
}

void FAbxrAuthService::StartReAuthPolling()
{
	if (ReAuthTickHandle.IsValid()) return;
	
	NextReAuthCheckAtSeconds = FPlatformTime::Seconds() + ReAuthPollSeconds;

	ReAuthTickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([AuthPtr = AsWeak()](float)
		{
			if (const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin()) return Self->ReAuthTick();
			
			return false;  // stop ticking. service is gone
		}),
		1.0f
	);
}

void FAbxrAuthService::StopReAuthPolling()
{
	if (ReAuthTickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(ReAuthTickHandle);
		ReAuthTickHandle.Reset();
	}
}

bool FAbxrAuthService::ReAuthTick()
{
	const double Now = FPlatformTime::Seconds();
	if (Now < NextReAuthCheckAtSeconds) return true;

	NextReAuthCheckAtSeconds = Now + ReAuthPollSeconds;

	if (TokenExpiry <= 0) return true;
	
	const int64 Remaining = TokenExpiry - FDateTime::UtcNow().ToUnixTimestamp();
	if (Remaining > ReAuthThresholdSeconds || bAttemptActive) return true;
	
	Authenticate();

	return true;
}

void FAbxrAuthService::AuthRequest(TFunction<void(bool)> OnComplete)
{
	if (bStopping || !bAttemptActive) { OnComplete(false); return; }
	if (Payload.SessionId.IsEmpty()) Payload.SessionId = FGuid::NewGuid().ToString();

	Payload.AuthMechanism = CreateAuthMechanismDict();

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Payload, Json);
	
	const TSharedPtr<int32> Attempt = MakeShared<int32>(1);
	TFunction<void()> DoAttempt;
	DoAttempt = [this, AuthPtr = AsWeak(), OnComplete, Json, Attempt, &DoAttempt]
	{
		const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self || Self->bStopping || !Self->bAttemptActive) return;

		const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
		Self->ActiveRequest = Request;
		Request->SetURL(AbxrUtil::CombineUrl(GetDefault<UAbxrSettings>()->RestUrl, TEXT("/v1/auth/token")));
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetContentAsString(Json);

		Request->OnProcessRequestComplete().BindLambda(
			[AuthPtr, OnComplete, Json, Attempt, DoAttempt](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bOk) mutable
			{
				const TSharedPtr<FAbxrAuthService> Self2 = AuthPtr.Pin();
				if (!Self2) return;

				Self2->ActiveRequest.Reset();
				if (Self2->bStopping || !Self2->bAttemptActive) return;

				const int32 Code = Response.IsValid() ? Response->GetResponseCode() : 0;

				// Success
				if (bOk && Response.IsValid() && EHttpResponseCodes::IsOk(Code))
				{
					const FString Body = Response->GetContentAsString();

					FAbxrAuthResponse AuthResponse;
					if (!FJsonObjectConverter::JsonObjectStringToUStruct<FAbxrAuthResponse>(Body, &AuthResponse, 0, 0))
					{
						UE_LOG(LogAbxrLib, Error, TEXT("Failed to parse auth response JSON: %s"), *Body);
						OnComplete(false);
						return;
					}

					Self2->ResponseData = AuthResponse;

					// Decode JWT exp safely (base64url + number or string)
					TArray<FString> Parts;
					Self2->ResponseData.Token.ParseIntoArray(Parts, TEXT("."));
					if (Parts.Num() >= 2)
					{
						FString PayloadB64Url = Parts[1];
						PayloadB64Url.ReplaceInline(TEXT("-"), TEXT("+"));
						PayloadB64Url.ReplaceInline(TEXT("_"), TEXT("/"));
						while (PayloadB64Url.Len() % 4 != 0) PayloadB64Url.AppendChar('=');

						FString DecodedPayloadJson;
						if (FBase64::Decode(PayloadB64Url, DecodedPayloadJson))
						{
							TSharedPtr<FJsonObject> PayloadJson;
							const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
							if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
							{
								const TSharedPtr<FJsonValue>* ValuePtr = PayloadJson->Values.Find(TEXT("exp"));
								if (ValuePtr && ValuePtr->IsValid())
								{
									int64 Exp = FCString::Atoi64(*(*ValuePtr)->AsString());
									Self2->TokenExpiry = static_cast<int32>(Exp);
								}
							}
						}
					}

					OnComplete(true);
					return;
				}

				// Failure
				const FString RespStr = Response.IsValid() ? Response->GetContentAsString() : TEXT("<no response>");
				UE_LOG(LogAbxrLib, Warning, TEXT("AuthRequest attempt failed: %s"), *RespStr);
				
				if (ShouldRetry(bOk, Response) && *Attempt < RetryMaxAttempts)
				{
					(*Attempt)++;
					UE_LOG(LogAbxrLib, Log, TEXT("AuthRequest retrying in %ds (attempt %d/%d)"), RetryDelaySeconds, *Attempt, RetryMaxAttempts);
					Self2->ScheduleRetry([DoAttempt]() mutable { DoAttempt(); });
					return;
				}
				
				if (*Attempt >= RetryMaxAttempts) UE_LOG(LogAbxrLib, Error, TEXT("AuthRequest failed (no more retries)"));
				OnComplete(false);
			});

		Request->ProcessRequest();
	};

	DoAttempt();
}

void FAbxrAuthService::GetConfiguration(TFunction<void(bool)> OnComplete)
{
	if (bStopping || !bAttemptActive) { OnComplete(false); return; }
	
	const TSharedPtr<int32> Attempt = MakeShared<int32>(1);
	TFunction<void()> DoAttempt;
	DoAttempt = [this, AuthPtr = AsWeak(), OnComplete, Attempt, &DoAttempt]
	{
		const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self || Self->bStopping || !Self->bAttemptActive) return;

		const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
		Self->ActiveRequest = Request;
		Request->SetURL(AbxrUtil::CombineUrl(GetDefault<UAbxrSettings>()->RestUrl, TEXT("/v1/storage/config")));
		Request->SetVerb(TEXT("GET"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Self->SetAuthHeaders(Request);

		Request->OnProcessRequestComplete().BindLambda(
			[AuthPtr, OnComplete, Attempt, DoAttempt](FHttpRequestPtr, const FHttpResponsePtr& Resp, const bool bOk) mutable
			{
				const TSharedPtr<FAbxrAuthService> Self2 = AuthPtr.Pin();
				if (!Self2) return;

				Self2->ActiveRequest.Reset();
				if (Self2->bStopping || !Self2->bAttemptActive) return;

				const int32 Code = Resp.IsValid() ? Resp->GetResponseCode() : 0;
				if (bOk && Resp.IsValid() && EHttpResponseCodes::IsOk(Code))
				{
					FAbxrConfigPayload Config;
					FJsonObjectConverter::JsonObjectStringToUStruct(*Resp->GetContentAsString(), &Config, 0, 0);
					Self2->SetConfigFromPayload(Config);
					Self2->AuthMechanism = Config.AuthMechanism;
					UE_LOG(LogAbxrLib, Log, TEXT("GetConfiguration() successful"));
					OnComplete(true);
					return;
				}

				const FString RespStr = Resp.IsValid() ? Resp->GetContentAsString() : TEXT("<no response>");
				UE_LOG(LogAbxrLib, Warning, TEXT("GetConfiguration attempt failed: %s"), *RespStr);
				
				if (ShouldRetry(bOk, Resp) && *Attempt < RetryMaxAttempts)
				{
					(*Attempt)++;
					UE_LOG(LogAbxrLib, Log, TEXT("GetConfiguration retrying in %ds (attempt %d/%d)"), RetryDelaySeconds, *Attempt, RetryMaxAttempts);
					Self2->ScheduleRetry([DoAttempt]() mutable { DoAttempt(); });
					return;
				}

				UE_LOG(LogAbxrLib, Error, TEXT("GetConfiguration failed (no more retries)"));
				OnComplete(false);
			});

		Request->ProcessRequest();
	};

	DoAttempt();
}

void FAbxrAuthService::SetConfigFromPayload(const FAbxrConfigPayload& Payload)
{
	UAbxrSettings* Config = GetMutableDefault<UAbxrSettings>();
    if (!Payload.RestUrl.IsEmpty()) Config->SetRestUrl(Payload.RestUrl);
	if (!Payload.SendRetriesOnFailure.IsEmpty()) Config->SetSendRetriesOnFailure(FCString::Atoi(*Payload.SendRetriesOnFailure));
	if (!Payload.SendRetryInterval.IsEmpty()) Config->SetSendRetryIntervalSeconds(FCString::Atoi(*Payload.SendRetryInterval));
	if (!Payload.SendNextBatchWait.IsEmpty()) Config->SetSendNextBatchWaitSeconds(FCString::Atoi(*Payload.SendNextBatchWait));
	if (!Payload.StragglerTimeout.IsEmpty()) Config->SetStragglerTimeoutSeconds(FCString::Atoi(*Payload.StragglerTimeout));
	if (!Payload.DataEntriesPerSendAttempt.IsEmpty()) Config->SetDataEntriesPerSendAttempt(FCString::Atoi(*Payload.DataEntriesPerSendAttempt));
	if (!Payload.StorageEntriesPerSendAttempt.IsEmpty()) Config->SetStorageEntriesPerSendAttempt(FCString::Atoi(*Payload.StorageEntriesPerSendAttempt));
	if (!Payload.PruneSentItemsOlderThan.IsEmpty()) Config->SetPruneSentItemsOlderThanHours(FCString::Atoi(*Payload.PruneSentItemsOlderThan));
	if (!Payload.MaximumCachedItems.IsEmpty()) Config->SetMaximumCachedItems(FCString::Atoi(*Payload.MaximumCachedItems));
	if (!Payload.RetainLocalAfterSent.IsEmpty()) Config->SetRetainLocalAfterSent(Payload.RetainLocalAfterSent.ToBool());
}

void FAbxrAuthService::GetConfigData()
{
	Payload.AppId = GetDefault<UAbxrSettings>()->AppId;
	Payload.OrgId = GetDefault<UAbxrSettings>()->OrgId;
	Payload.AuthSecret = GetDefault<UAbxrSettings>()->AuthSecret;
}

void FAbxrAuthService::GetArborData()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		Payload.OrgId = XRDMService->GetOrgId();
		Payload.Partner = TEXT("arborxr");
		Payload.DeviceId = XRDMService->GetDeviceId();
		Payload.Tags = XRDMService->GetDeviceTags();
		Payload.AuthSecret = XRDMService->GetFingerprint();
	}
}

void FAbxrAuthService::AuthSucceeded()
{
	bAttemptActive = false;
	bAuthenticated = true;
	Callbacks.OnSucceeded();
	UE_LOG(LogAbxrLib, Log, TEXT("Authenticated successfully"));
}

void FAbxrAuthService::KeyboardAuthenticate()
{
	FString Prompt = TEXT("");
	if (FailedAuthAttempts > 0) Prompt = TEXT("Authentication Failed (") + FString::FromInt(FailedAuthAttempts) + ")\n";
	Prompt.Append(AuthMechanism.Prompt);

	// Emit an input request. This may fire multiple times as the user retries.
	Callbacks.OnInputRequested(FAbxrAuthMechanism(AuthMechanism.Type, Prompt, AuthMechanism.Domain));
	
	FailedAuthAttempts++;
}

void FAbxrAuthService::KeyboardAuthenticate(const FString& KeyboardInput)
{
	FString OriginalPrompt = AuthMechanism.Prompt;
	AuthMechanism.Prompt = KeyboardInput;
	AuthRequest([AuthPtr = AsWeak(), OriginalPrompt](const bool bSuccess)
	{
		const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self) return;
		if (bSuccess)
		{
			Self->AuthSucceeded();
		}
		else
		{
			Self->AuthMechanism.Prompt = OriginalPrompt;
			Self->KeyboardAuthenticate();
		}
	});
}


void FAbxrAuthService::SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json) const
{
	Request->SetHeader("Authorization", "Bearer " + ResponseData.Token);

	const FString UnixTime = LexToString(FDateTime::UtcNow().ToUnixTimestamp());
	Request->SetHeader("x-abxrlib-timestamp", UnixTime);

	FString HashString = ResponseData.Token + ResponseData.Secret + UnixTime;
	if (!Json.IsEmpty())
	{
		const uint32 CRC = AbxrUtil::ComputeCRC32(Json);
		HashString += LexToString(CRC);
	}
	
	Request->SetHeader("x-abxrlib-hash", AbxrUtil::ComputeSHA256(HashString));
}

TMap<FString, FString> FAbxrAuthService::CreateAuthMechanismDict() const
{
	TMap<FString, FString> Dict;
	if (!AuthMechanism.Type.IsEmpty()) Dict.Add(TEXT("type"), AuthMechanism.Type);
	if (!AuthMechanism.Prompt.IsEmpty()) Dict.Add(TEXT("prompt"), AuthMechanism.Prompt);
	if (!AuthMechanism.Domain.IsEmpty()) Dict.Add(TEXT("domain"), AuthMechanism.Domain);
	return Dict;
}

void FAbxrAuthService::ClearAuthenticationState()
{
	bAuthenticated = false;
	ResponseData = FAbxrAuthResponse();
	TokenExpiry = 0;
	Payload.SessionId = TEXT("");
	AuthMechanism = FAbxrAuthMechanism();
	FailedAuthAttempts = 0;
	UE_LOG(LogAbxrLib, Log, TEXT("Authentication state cleared - data transmission stopped"));
}
