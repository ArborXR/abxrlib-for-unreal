#include "AbxrLib/Public/Authentication.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

FString Authentication::AuthToken;
FString Authentication::ApiSecret;
FString Authentication::SessionId;
int Authentication::TokenExpiry;

void Authentication::Authenticate()
{
	FAuthPayload Payload;
	Payload.appId = TEXT("d301143a-1481-41f2-b7ae-db3c55e18a35");
	Payload.orgId = TEXT("08183be6-efd4-4814-83d1-ed88db8b4918");
	Payload.authSecret = TEXT("WXOg9L0v2O9cah3jI1JsQKDBddX9kmd0B_M6-UmzobmwJGgxWV6iU6IQWky5JQ2Q");
	Payload.deviceId = TEXT("34f9f880-8360-47a2-89c8-ddccb6652f82");
	Payload.userId = TEXT("userid");
	Payload.tags = { };
	Payload.sessionId = TEXT("25b34140-62c9-4c33-b3e6-3fe9cabd50d4");
	Payload.partner = TEXT("none");
	Payload.ipAddress = TEXT("");
	Payload.deviceModel = TEXT("");
	Payload.geolocation = TMap<FString, FString>();
	Payload.osVersion = TEXT("1.0");
	Payload.xrdmVersion = TEXT("1.0");
	Payload.appVersion = TEXT("1.0");
	Payload.unrealVersion = TEXT("1.0");
	Payload.abxrLibVersion = TEXT("1.0");
	Payload.authMechanism = CreateAuthMechanismDict();

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Payload, Json);

	UE_LOG(LogTemp, Warning, TEXT("JSON Output: %s"), *Json);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://lib-backend.xrdm.app/v1/auth/token"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
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
			FString PayloadBase64 = Parts[1];

			FString DecodedPayloadJson;
			FBase64::Decode(PayloadBase64, DecodedPayloadJson);

			TSharedPtr<FJsonObject> PayloadJson;
			Reader = TJsonReaderFactory<>::Create(DecodedPayloadJson);
			if (FJsonSerializer::Deserialize(Reader, PayloadJson) && PayloadJson.IsValid())
			{
				ValuePtr = PayloadJson->Values.Find("exp");
				FString Expiry = (*ValuePtr)->AsString();
				TokenExpiry = FCString::Atoi(*Expiry);
			}

			UE_LOG(LogTemp, Log, TEXT("Token: %s"), *AuthToken);
			UE_LOG(LogTemp, Log, TEXT("Epiry: %i"), TokenExpiry);
		}
	});
	
	Request->ProcessRequest();
}

TMap<FString, FString> Authentication::CreateAuthMechanismDict()
{
	TMap<FString, FString> dict;
	//if (_authMechanism == null) return dict;
        
	//if (!string.IsNullOrEmpty(_authMechanism.type)) dict["type"] = _authMechanism.type;
	//if (!string.IsNullOrEmpty(_authMechanism.prompt)) dict["prompt"] = _authMechanism.prompt;
	//if (!string.IsNullOrEmpty(_authMechanism.domain)) dict["domain"] = _authMechanism.domain;
	return dict;
}

