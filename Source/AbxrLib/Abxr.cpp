#include "Abxr.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

void UAbxr::SendEvent(const FString& Endpoint, const FString& EventName, const FString& PayloadJson)
{
    TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
    RootObject->SetStringField("event", EventName);

    TSharedPtr<FJsonObject> PayloadObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PayloadJson);
    if (FJsonSerializer::Deserialize(Reader, PayloadObject) && PayloadObject.IsValid())
    {
        RootObject->SetObjectField("payload", PayloadObject);
    }
    else
    {
        RootObject->SetStringField("payload_raw", PayloadJson);
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject, Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Endpoint);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(OutputString);
    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
    {
        if (bSuccess && Resp.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("Event sent successfully: %s"), *Resp->GetContentAsString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to send event"));
        }
    });
    Request->ProcessRequest();
}

#include "Modules/ModuleManager.h"
IMPLEMENT_MODULE(FDefaultModuleImpl, AbxrLib)
