#include "AbxrLib/Public/Authentication.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"


void Authentication::Authenticate()
{
	UE_LOG(LogTemp, Warning, TEXT("Authenticate CALLED!!!"));
	
	/*FAuthPayload Payload;
	Payload.appId = TEXT("appid");
	Payload.orgId = TEXT("orgid");
	Payload.authSecret = TEXT("authsecret");
	Payload.deviceId = TEXT("deviceid");
	Payload.userId = TEXT("userid");
	Payload.tags = { TEXT("Admin"), TEXT("User"), TEXT("Editor") };
	Payload.sessionId = TEXT("sessionid");
	Payload.partner = TEXT("partner");
	Payload.ipAddress = TEXT("ipaddress");
	Payload.deviceModel = TEXT("devicemodel");
	Payload.geolocation = TMap<FString, FString>();
	Payload.osVersion = TEXT("osversion");
	Payload.xrdmVersion = TEXT("xrdmversion");
	Payload.appVersion = TEXT("appversion");
	Payload.unrealVersion = TEXT("unrealversion");
	Payload.abxrLibVersion = TEXT("abxrlibversion");
	Payload.authMechanism = CreateAuthMechanismDict();

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Payload, Json);

	UE_LOG(LogTemp, Warning, TEXT("JSON Output: %s"), *Json);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://lib-backend.xrdm.app/"));
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

		// ✅ Response code (e.g., 200 OK)
		int32 StatusCode = Response->GetResponseCode();
		UE_LOG(LogTemp, Log, TEXT("Response Code: %d"), StatusCode);

		// ✅ Response body (as string)
		FString Body = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response Body: %s"), *Body);

		// ✅ Optional: Parse as JSON
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			FString Token = JsonObject->GetStringField("access_token");
			UE_LOG(LogTemp, Log, TEXT("Access Token: %s"), *Token);
		}
	});
	
	Request->ProcessRequest();*/
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

