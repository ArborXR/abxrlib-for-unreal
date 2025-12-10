#include "Authentication.h"
#include "Abxr.h"
#include "AbxrLibConfiguration.h"
#include "Utils.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "XRDMService.h"
#include "Interfaces/IHttpResponse.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "HAL/PlatformMisc.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Launch/Resources/Version.h"

FString Authentication::SessionId;
int Authentication::TokenExpiry;
FAuthMechanism Authentication::AuthMechanism;
int Authentication::FailedAuthAttempts = 0;
std::optional<bool> Authentication::NeedKeyboardAuth;
FString Authentication::OrgId;
FString Authentication::DeviceId;
FString Authentication::AuthSecret;
FString Authentication::AppId;
FString Authentication::Partner = "none";
FString Authentication::DeviceModel;
TArray<FString> Authentication::DeviceTags;
FString Authentication::XrdmVersion;
FString Authentication::IpAddress;
std::thread Authentication::ReAuthThread;
std::atomic<bool> Authentication::bShouldStop{false};
FAuthResponse Authentication::ResponseData;

void Authentication::Authenticate()
{
	if (!GetDefault<UAbxrLibConfiguration>()->IsValid()) return;
	GetConfigData();
	DeviceId = FGuid::NewGuid().ToString();
#if PLATFORM_ANDROID
	const auto Promise = UXRDMService::GetInstance()->WaitForConnection();
	Promise->GetFuture().Next([](bool bConnected)
	{
		if (bConnected) GetArborData();
#endif
		AuthRequest([](const bool bSuccess)
		{
			if (bSuccess)
			{
				GetConfiguration([](const bool)
				{
					if (!AuthMechanism.Prompt.IsEmpty())
					{
#if !PLATFORM_ANDROID
						KeyboardAuthenticate();
#else
						NeedKeyboardAuth = false; // TODO can't do this yet
#endif
					}
					else
					{
						NeedKeyboardAuth = false;
						UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authentication fully completed"));
					}
				});
			}
		});
#if PLATFORM_ANDROID
	});
#endif
	PollForReAuth();
}

void Authentication::PollForReAuth()
{
	if (ReAuthThread.joinable()) return;  // Only start if not already running
	ReAuthThread = std::thread(&Authentication::ReAuthThreadFunction);
}

void Authentication::ReAuthThreadFunction()
{
	while (!bShouldStop.load())
	{
		std::this_thread::sleep_for(std::chrono::minutes(1));
		if (!bShouldStop.load()) CheckReauthentication();
	}
}

void Authentication::CheckReauthentication()
{
	if (TokenExpiry - FDateTime::UtcNow().ToUnixTimestamp() <= 120)
	{
		// Clear authentication state to stop data transmission
		ClearAuthenticationState();
		
		// Marshal back to game thread
		AsyncTask(ENamedThreads::GameThread, []
		{
			AuthRequest([](const bool){});
		});
	}
}

void Authentication::AuthRequest(TFunction<void(bool)> OnComplete)
{
	if (SessionId.IsEmpty()) SessionId = FGuid::NewGuid().ToString();

	FString HMDName = TEXT("None");
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();
	}
	
	FAuthPayload Payload;
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
	Request->SetURL(Utils::CombineUrl(GetDefault<UAbxrLibConfiguration>()->RestUrl, TEXT("/v1/auth/token")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	
	Request->OnProcessRequestComplete().BindLambda([OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib: Authentication failed : %s"), *Response->GetContentAsString());
			OnComplete(false);
			return;
		}
		
		const FString Body = Response->GetContentAsString();

		FAuthResponse AuthResponse;
		if (!FJsonObjectConverter::JsonObjectStringToUStruct<FAuthResponse>(Body, &AuthResponse, 0, 0))
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib: Failed to parse auth response JSON: %s"), *Body);
			OnComplete(false);
			return;
		}
		
		ResponseData = AuthResponse;

		TArray<FString> Parts;
		ResponseData.Token.ParseIntoArray(Parts, TEXT("."));
		const FString PayloadBase64 = Parts[1];

		FString DecodedPayloadJson;
		FBase64::Decode(PayloadBase64, DecodedPayloadJson);

		TSharedPtr<FJsonObject> PayloadJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
		if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
		{
			const TSharedPtr<FJsonValue>* ValuePtr = PayloadJson->Values.Find("exp");
			const FString Expiry = (*ValuePtr)->AsString();
			TokenExpiry = FCString::Atoi(*Expiry);
		}
		
		OnComplete(true);
		UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authenticated successfully"));
	});
	
	Request->ProcessRequest();
}

void Authentication::GetConfiguration(TFunction<void(bool)> OnComplete)
{
	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Utils::CombineUrl(GetDefault<UAbxrLibConfiguration>()->RestUrl, TEXT("/v1/storage/config")));
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	SetAuthHeaders(Request);

	Request->OnProcessRequestComplete().BindLambda([OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Resp, const bool bOk)
	{
		if (bOk && Resp.IsValid())
		{
			FConfigPayload Config;
			FJsonObjectConverter::JsonObjectStringToUStruct(*Resp->GetContentAsString(), &Config, 0, 0);
			SetConfigFromPayload(Config);
			AuthMechanism = Config.AuthMechanism;
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

void Authentication::SetConfigFromPayload(const FConfigPayload& Payload)
{
	UAbxrLibConfiguration* Config = GetMutableDefault<UAbxrLibConfiguration>();
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

void Authentication::GetConfigData()
{
	AppId = GetDefault<UAbxrLibConfiguration>()->AppId;
	OrgId = GetDefault<UAbxrLibConfiguration>()->OrgId;
	AuthSecret = GetDefault<UAbxrLibConfiguration>()->AuthSecret;
}

void Authentication::GetArborData()
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

void Authentication::KeyboardAuthenticate()
{
	NeedKeyboardAuth = true;
	FString Prompt = TEXT("");
	if (FailedAuthAttempts > 0) Prompt = TEXT("Authentication Failed (") + FString::FromInt(FailedAuthAttempts) + ")\n";
	Prompt.Append(AuthMechanism.Prompt);
	UAbxr::PresentKeyboard(Prompt, AuthMechanism.Type, AuthMechanism.Domain);
	FailedAuthAttempts++;
}

void Authentication::KeyboardAuthenticate(const FString& KeyboardInput)
{
	FString OriginalPrompt = AuthMechanism.Prompt;
	AuthMechanism.Prompt = KeyboardInput;
	AuthRequest([OriginalPrompt](const bool bSuccess)
	{
		if (bSuccess)
		{
			NeedKeyboardAuth = false;
			FailedAuthAttempts = 0;
		}
		else
		{
			AuthMechanism.Prompt = OriginalPrompt;
			KeyboardAuthenticate();
		}
	});
}


void Authentication::SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json)
{
	Request->SetHeader("Authorization", "Bearer " + ResponseData.Token);

	const FString UnixTime = LexToString(FDateTime::UtcNow().ToUnixTimestamp());
	Request->SetHeader("x-abxrlib-timestamp", UnixTime);

	FString HashString = ResponseData.Token + ResponseData.Secret + UnixTime;
	if (!Json.IsEmpty())
	{
		const uint32 CRC = Utils::ComputeCRC32(Json);
		HashString += LexToString(CRC);
	}
	
	Request->SetHeader("x-abxrlib-hash", Utils::ComputeSHA256(HashString));
}

TMap<FString, FString> Authentication::CreateAuthMechanismDict()
{
	TMap<FString, FString> Dict;
	if (!AuthMechanism.Type.IsEmpty()) Dict.Add(TEXT("type"), AuthMechanism.Type);
	if (!AuthMechanism.Prompt.IsEmpty()) Dict.Add(TEXT("prompt"), AuthMechanism.Prompt);
	if (!AuthMechanism.Domain.IsEmpty()) Dict.Add(TEXT("domain"), AuthMechanism.Domain);
	return Dict;
}

void Authentication::ClearAuthenticationState()
{
	ResponseData = FAuthResponse();
	TokenExpiry = 0;
	NeedKeyboardAuth.reset();
	SessionId = TEXT("");
	AuthMechanism = FAuthMechanism();

	// Clear cached user data
	//_authResponseModuleData = null;

	// Clear stored auth value
	//_enteredAuthValue = null;
	
	FailedAuthAttempts = 0;

	// Reset auth handoff tracking
	//_sessionUsedAuthHandoff = false;

	UE_LOG(LogTemp, Log, TEXT("AbxrLib: Authentication state cleared - data transmission stopped"));
}
