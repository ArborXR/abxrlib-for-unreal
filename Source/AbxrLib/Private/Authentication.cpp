#include "Authentication.h"
#include "Abxr.h"
#include "AbxrLibConfiguration.h"
#include "Utils.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "HAL/PlatformMisc.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Launch/Resources/Version.h"

FString Authentication::AuthToken;
FString Authentication::ApiSecret;
FString Authentication::SessionId;
int Authentication::TokenExpiry;
FAuthMechanism Authentication::AuthMechanism;
int Authentication::FailedAuthAttempts = 0;
bool Authentication::KeyboardAuthSuccess = false;

void Authentication::Authenticate()
{
	Reset();
	AuthRequest([](const bool bSuccess)
	{
		if (bSuccess)
		{
			GetConfiguration([](const bool)
			{
				if (!AuthMechanism.prompt.IsEmpty())
				{
					KeyboardAuthenticate();
				}
			});
		}
	});
}

void Authentication::AuthRequest(TFunction<void(bool)> OnComplete)
{
	KeyboardAuthSuccess = false;
	
	FAuthPayload Payload;
	Payload.appId = GetDefault<UAbxrLibConfiguration>()->AppId;
	Payload.orgId = GetDefault<UAbxrLibConfiguration>()->OrgId;
	Payload.authSecret = GetDefault<UAbxrLibConfiguration>()->AuthSecret;
	Payload.deviceId = TEXT("34f9f880-8360-47a2-89c8-ddccb6652f82");
	Payload.userId = TEXT("userid");
	Payload.tags = { };
	Payload.sessionId = TEXT("25b34140-62c9-4c33-b3e6-3fe9cabd50d4");
	Payload.partner = TEXT("none");
	Payload.ipAddress = TEXT("");
	Payload.deviceModel = TEXT("");
	Payload.geolocation = TMap<FString, FString>();
	Payload.osVersion = FPlatformMisc::GetOSVersion();
	Payload.xrdmVersion = TEXT("1.0");
	Payload.appVersion = TEXT("1.0");
	Payload.unrealVersion = FString::Printf(TEXT("%d.%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);
	Payload.abxrLibVersion = IPluginManager::Get().FindPlugin(TEXT("AbxrLib"))->GetDescriptor().VersionName;
	Payload.authMechanism = CreateAuthMechanismDict();

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Payload, Json);
	UE_LOG(LogTemp, Error, TEXT("AbxrLib - JSON: %s"), *Json);

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Utils::CombineUrl(GetDefault<UAbxrLibConfiguration>()->RestUrl, TEXT("/v1/auth/token")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	
	Request->OnProcessRequestComplete().BindLambda([OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Authentication failed : %s"), *Response->GetContentAsString());
			OnComplete(false);
			return;
		}
		
		const FString Body = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			JsonObject->TryGetStringField(TEXT("token"), AuthToken);
			JsonObject->TryGetStringField(TEXT("secret"), ApiSecret);

			TArray<FString> Parts;
			AuthToken.ParseIntoArray(Parts, TEXT("."));
			const FString PayloadBase64 = Parts[1];

			FString DecodedPayloadJson;
			FBase64::Decode(PayloadBase64, DecodedPayloadJson);

			TSharedPtr<FJsonObject> PayloadJson;
			Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
			if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
			{
				FString ExpStr;
				if (JsonObject->TryGetStringField(TEXT("exp"), ExpStr))
				{
					TokenExpiry = FCString::Atoi64(*ExpStr);
				}
			}
			
			KeyboardAuthSuccess = true;
			OnComplete(true);
		}
	});
	
	Request->ProcessRequest();
}

void Authentication::GetConfiguration(TFunction<void(bool)> OnComplete)
{
	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://lib-backend.xrdm.app/v1/storage/config");
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	SetAuthHeaders(Request);

	Request->OnProcessRequestComplete().BindLambda([OnComplete](FHttpRequestPtr, const FHttpResponsePtr& Resp, const bool bOk)
	{
		if (bOk && Resp.IsValid())
		{
			FConfigPayload Config;
			FJsonObjectConverter::JsonObjectStringToUStruct(*Resp->GetContentAsString(), &Config, 0, 0);
			AuthMechanism = Config.authMechanism;
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

void Authentication::KeyboardAuthenticate()
{
	FString Prompt = TEXT("");
	if (FailedAuthAttempts > 0) Prompt = TEXT("Authentication Failed (") + FString::FromInt(FailedAuthAttempts) + ")\n";
	Prompt.Append(AuthMechanism.prompt);
	UAbxr::PresentKeyboard(Prompt, AuthMechanism.type, AuthMechanism.domain);
	FailedAuthAttempts++;
}

void Authentication::KeyboardAuthenticate(const FString& KeyboardInput)
{
	FString OriginalPrompt = AuthMechanism.prompt;
	AuthMechanism.prompt = KeyboardInput;
	AuthRequest([OriginalPrompt](const bool bSuccess)
	{
		if (bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - IN BREAKOUT!"));
			//KeyboardHandler.Destroy();
			FailedAuthAttempts = 0;
		}
		else
		{
			AuthMechanism.prompt = OriginalPrompt;
			KeyboardAuthenticate();
		}
	});
}


void Authentication::SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json)
{
	Request->SetHeader("Authorization", "Bearer " + AuthToken);

	const FString UnixTime = LexToString(FDateTime::UtcNow().ToUnixTimestamp());
	Request->SetHeader("x-abxrlib-timestamp", UnixTime);

	FString HashString = AuthToken + ApiSecret + UnixTime;
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
	if (!AuthMechanism.type.IsEmpty()) Dict.Add(TEXT("type"), AuthMechanism.type);
	if (!AuthMechanism.prompt.IsEmpty()) Dict.Add(TEXT("prompt"), AuthMechanism.prompt);
	if (!AuthMechanism.domain.IsEmpty()) Dict.Add(TEXT("domain"), AuthMechanism.domain);
	return Dict;
}

// TODO maybe this auth class should be static and Unreal prefers to store the objects somewhere
void Authentication::Reset()
{
	AuthToken = TEXT("");
	ApiSecret = TEXT("");
	SessionId = TEXT("");
	TokenExpiry = 0;
	AuthMechanism = FAuthMechanism();
	FailedAuthAttempts = 0;
	KeyboardAuthSuccess = false;
}

