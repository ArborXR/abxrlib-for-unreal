#include "Authentication.h"
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

void Authentication::Authenticate()
{
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

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Authentication failed : %s"), *Response->GetContentAsString());
			return;
		}

		const FString Body = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const TSharedPtr<FJsonValue>* ValuePtr = JsonObject->Values.Find("token");
			AuthToken = (*ValuePtr)->AsString();

			ValuePtr = JsonObject->Values.Find("secret");
			ApiSecret = (*ValuePtr)->AsString();

			TArray<FString> Parts;
			AuthToken.ParseIntoArray(Parts, TEXT("."));
			const FString PayloadBase64 = Parts[1];

			FString DecodedPayloadJson;
			FBase64::Decode(PayloadBase64, DecodedPayloadJson);

			TSharedPtr<FJsonObject> PayloadJson;
			Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
			if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
			{
				ValuePtr = PayloadJson->Values.Find("exp");
				const FString Expiry = (*ValuePtr)->AsString();
				TokenExpiry = FCString::Atoi(*Expiry);
			}
			
			TSharedRef<IHttpRequest> ConfigRequest = FHttpModule::Get().CreateRequest();
			ConfigRequest->SetURL("https://lib-backend.xrdm.app/v1/storage/config");
			ConfigRequest->SetVerb("GET");
			ConfigRequest->SetHeader("Content-Type", "application/json");
			SetAuthHeaders(ConfigRequest);

			ConfigRequest->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, const FHttpResponsePtr& Resp, const bool bOk)
			{
				if (bOk && Resp.IsValid())
				{
					FConfigPayload Config;
					FJsonObjectConverter::JsonObjectStringToUStruct(*Resp->GetContentAsString(), &Config, 0, 0);
					AuthMechanism = Config.authMechanism;
					if (!AuthMechanism.prompt.IsEmpty())
					{
						KeyboardAuthenticate();
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("AbxrLib - GetConfiguration failed: %s"), *Resp->GetContentAsString());
				}
			});

			ConfigRequest->ProcessRequest();
		}
	});
	
	Request->ProcessRequest();
}

void Authentication::KeyboardAuthenticate()
{
	
}

void Authentication::KeyboardAuthenticate(const FString& Input)
{
	
}


void Authentication::SetAuthHeaders(const TSharedRef<IHttpRequest>& Request, const FString& Json)
{
	Request->SetHeader("Authorization", "Bearer " + AuthToken);

	FString UnixTime = LexToString(FDateTime::UtcNow().ToUnixTimestamp());
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

