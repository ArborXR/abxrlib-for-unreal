#include "EventBatcher.h"
#include "Authentication.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "Containers/Array.h"
#include "HAL/CriticalSection.h"

FCriticalSection Mutex;

TArray<FPayload> Payloads;

void EventBatcher::Add(FString Name, const TMap<FString, FString>& Meta)
{
	FDateTime Now = FDateTime::UtcNow(); //TODO should this be using Game Time (would need context)
	int64 UnixMillis = Now.ToUnixTimestamp() * 1000 + Now.GetMillisecond();
	FString PreciseTimestamp = FString::Printf(TEXT("%lld"), UnixMillis);

	FPayload Payload;
	Payload.preciseTimestamp = PreciseTimestamp;
	Payload.name = Name;
	Payload.meta = Meta;

	{
		FScopeLock Lock(&Mutex);
		Payloads.Add(Payload);
		if (Payloads.Num() >= 5) // TODO get from config
		{
			// Set timer to 0 so it triggers a send
		}
	}
}

void EventBatcher::Send()
{
	//if (Time.time - _lastCallTime < MaxCallFrequencySeconds) yield break;
		
	//_lastCallTime = Time.time;
	//_timer = Configuration.Instance.sendNextBatchWaitSeconds; // reset timer
	//if (!Authentication::Authenticated()) return;
	
	{
		FScopeLock Lock(&Mutex);
		if (Payloads.Num() == 0) return;
	}

	TArray<FPayload> EventsToSend;
	{
		FScopeLock Lock(&Mutex);
		EventsToSend = MoveTemp(Payloads);
	}

	FPayloadWrapper Wrapper;
	Wrapper.data = EventsToSend;

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(FPayloadWrapper::StaticStruct(), &Wrapper, Json, 0, 0, 0, nullptr, false);

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://lib-backend.xrdm.app/v1/collect/event"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	Authentication::SetAuthHeaders(Request, Json);

	Request->OnProcessRequestComplete().BindLambda([EventsToSend](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Event POST Request failed : %s"), *Response->GetContentAsString());
			//_timer = Configuration.Instance.sendRetryIntervalSeconds;
			{
				FScopeLock Lock(&Mutex);
				Payloads.Insert(EventsToSend, 0);
			}
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("AbxrLib - Event POST Request successful"));
	});
	
	Request->ProcessRequest();
}
