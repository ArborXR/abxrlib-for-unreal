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

void FAbxrAuthService::Authenticate()
{
	if (!GetDefault<UAbxrSettings>()->IsValid()) return;
	GetConfigData();
	DeviceId = FGuid::NewGuid().ToString();
	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
#if PLATFORM_ANDROID
	const auto Promise = UXRDMService::GetInstance()->WaitForConnection();
	Promise->GetFuture().Next([AuthPtr](const bool bConnected)
	{
		const TSharedPtr<FAbxrAuthService> Self1 = AuthPtr.Pin();
		if (!Self1) return;
		if (bConnected) Self1->GetArborData();
#else
		const TSharedPtr<FAbxrAuthService> Self1 = AuthPtr.Pin();
		if (!Self1) return;
#endif
		Self1->AuthRequest([AuthPtr](const bool bSuccess) // TODO this passed in OK?
		{
			const TSharedPtr<FAbxrAuthService> Self2 = AuthPtr.Pin();
			if (!Self2) return;
			if (bSuccess)
			{
				Self2->GetConfiguration([AuthPtr](const bool)
				{
					const TSharedPtr<FAbxrAuthService> Self3 = AuthPtr.Pin();
					if (!Self3) return;
					if (!Self3->AuthMechanism.Prompt.IsEmpty())
					{
						Self3->KeyboardAuthenticate();
					}
					else
					{
						Self3->NeedKeyboardAuth = false;
						UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authentication fully completed"));
					}
				});
				
				Self2->StartReAuthPolling();
			}
		});
#if PLATFORM_ANDROID
	});
#endif
}

void FAbxrAuthService::StartReAuthPolling()
{
	if (ReauthTickHandle.IsValid()) return;

	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
	
	NextReauthCheckAtSeconds = FPlatformTime::Seconds() + ReauthPollSeconds;

	ReauthTickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([AuthPtr](float)
		{
			if (const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin()) return Self->ReAuthTick();
			
			return false;  // stop ticking. service is gone
		}),
		1.0f
	);
}

void FAbxrAuthService::StopReAuthPolling()
{
	if (ReauthTickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(ReauthTickHandle);
		ReauthTickHandle.Reset();
	}
}

bool FAbxrAuthService::ReAuthTick()
{
	const double Now = FPlatformTime::Seconds();
	if (Now < NextReauthCheckAtSeconds) return true;

	NextReauthCheckAtSeconds = Now + ReauthPollSeconds;

	if (TokenExpiry <= 0) return true;
	
	const int64 Remaining = TokenExpiry - FDateTime::UtcNow().ToUnixTimestamp();

	if (Remaining > ReauthThresholdSeconds || bReauthInFlight) return true;

	bReauthInFlight = true;
	
	// Clear authentication state to stop data transmission
	ClearAuthenticationState();
	
	// Marshal back to game thread
	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
	AsyncTask(ENamedThreads::GameThread, [AuthPtr]
	{
		if (const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin())
		{
			Self->AuthRequest([](const bool){});
		}
	});

	return true;
}

void FAbxrAuthService::AuthRequest(TFunction<void(bool)> OnComplete)
{
	if (SessionId.IsEmpty()) SessionId = FGuid::NewGuid().ToString();

	FString HMDName = TEXT("None");
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();
	}

	FAbxrAuthPayload Payload;
	Payload.AppId = AppId;
	Payload.OrgId = OrgId;
	Payload.AuthSecret = AuthSecret;
	Payload.DeviceId = DeviceId;
	Payload.Tags = { };
	Payload.SessionId = SessionId;
	Payload.Partner = Partner;
	Payload.IpAddress = TEXT("");
	Payload.DeviceModel = HMDName;
	Payload.Geolocation = TMap<FString, FString>();
	Payload.OsVersion = FPlatformMisc::GetOSVersion();
	Payload.XrdmVersion = TEXT("1.0.0");
	Payload.AppVersion = TEXT("1.0.0");
	Payload.UnrealVersion = FString::Printf(TEXT("%d.%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);
	Payload.AbxrLibVersion = IPluginManager::Get().FindPlugin(TEXT("AbxrLib"))->GetDescriptor().VersionName;
	Payload.AuthMechanism = CreateAuthMechanismDict();

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Payload, Json);

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(AbxrUtil::CombineUrl(GetDefault<UAbxrSettings>()->RestUrl, TEXT("/v1/auth/token")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	
	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
	Request->OnProcessRequestComplete().BindLambda([AuthPtr, OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self) return;
		if (!bWasSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib: Authentication failed : %s"), *Response->GetContentAsString());
			OnComplete(false);
			return;
		}
		
		const FString Body = Response->GetContentAsString();

		FAbxrAuthResponse AuthResponse;
		if (!FJsonObjectConverter::JsonObjectStringToUStruct<FAbxrAuthResponse>(Body, &AuthResponse, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib: Failed to parse auth response JSON: %s"), *Body);
			OnComplete(false);
			return;
		}
		
		Self->ResponseData = AuthResponse;

		TArray<FString> Parts;
		Self->ResponseData.Token.ParseIntoArray(Parts, TEXT("."));
		const FString PayloadBase64 = Parts[1];

		FString DecodedPayloadJson;
		FBase64::Decode(PayloadBase64, DecodedPayloadJson);

		TSharedPtr<FJsonObject> PayloadJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
		if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
		{
			const TSharedPtr<FJsonValue>* ValuePtr = PayloadJson->Values.Find("exp");
			const FString Expiry = (*ValuePtr)->AsString();
			Self->TokenExpiry = FCString::Atoi(*Expiry);
		}
		
		OnComplete(true);
		UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authenticated successfully"));
	});
	
	Request->ProcessRequest();
}

void FAbxrAuthService::GetConfiguration(TFunction<void(bool)> OnComplete)
{
	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(AbxrUtil::CombineUrl(GetDefault<UAbxrSettings>()->RestUrl, TEXT("/v1/storage/config")));
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	SetAuthHeaders(Request);

	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
	Request->OnProcessRequestComplete().BindLambda([AuthPtr, OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Resp, const bool bOk)
	{
		const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self) return;
		if (bOk && Resp.IsValid())
		{
			FAbxrConfigPayload Config;
			FJsonObjectConverter::JsonObjectStringToUStruct(*Resp->GetContentAsString(), &Config, 0, 0);
			Self->SetConfigFromPayload(Config);
			Self->AuthMechanism = Config.AuthMechanism;
			UE_LOG(LogTemp, Log, TEXT("AbxrLib - GetConfiguration() successful"));
			OnComplete(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - GetConfiguration failed: %s"), *Resp->GetContentAsString());
			OnComplete(false);
		}
	});

	Request->ProcessRequest();
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
	AppId = GetDefault<UAbxrSettings>()->AppId;
	OrgId = GetDefault<UAbxrSettings>()->OrgId;
	AuthSecret = GetDefault<UAbxrSettings>()->AuthSecret;
}

void FAbxrAuthService::GetArborData()
{
	if (UXRDMService* XRDMService = UXRDMService::GetInstance())
	{
		OrgId = XRDMService->GetOrgId();
		Partner = "arborxr";
		DeviceId = XRDMService->GetDeviceId();
		DeviceTags = XRDMService->GetDeviceTags();
		AuthSecret = XRDMService->GetFingerprint();
	}
}

void FAbxrAuthService::KeyboardAuthenticate()
{
	NeedKeyboardAuth = true;
	FString Prompt = TEXT("");
	if (FailedAuthAttempts > 0) Prompt = TEXT("Authentication Failed (") + FString::FromInt(FailedAuthAttempts) + ")\n";
	Prompt.Append(AuthMechanism.Prompt);
	Abxr::PresentKeyboard(Prompt, AuthMechanism.Type, AuthMechanism.Domain);
	FailedAuthAttempts++;
}

void FAbxrAuthService::KeyboardAuthenticate(const FString& KeyboardInput)
{
	FString OriginalPrompt = AuthMechanism.Prompt;
	AuthMechanism.Prompt = KeyboardInput;
	TWeakPtr<FAbxrAuthService> AuthPtr = AsWeak();
	AuthRequest([AuthPtr, OriginalPrompt](const bool bSuccess)
	{
		const TSharedPtr<FAbxrAuthService> Self = AuthPtr.Pin();
		if (!Self) return;
		if (bSuccess)
		{
			Self->NeedKeyboardAuth = false;
			Self->FailedAuthAttempts = 0;
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
	ResponseData = FAbxrAuthResponse();
	TokenExpiry = 0;
	NeedKeyboardAuth.reset();
	SessionId = TEXT("");
	AuthMechanism = FAbxrAuthMechanism();

	// Clear cached user data
	//_authResponseModuleData = null;

	// Clear stored auth value
	//_enteredAuthValue = null;
	
	FailedAuthAttempts = 0;

	// Reset auth handoff tracking
	//_sessionUsedAuthHandoff = false;

	UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authentication state cleared - data transmission stopped"));
}
