#include "TelemetryBatcher.h"
#include "AbxrLibConfiguration.h"
#include "Authentication.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Utils.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "Containers/Array.h"
#include "HAL/CriticalSection.h"
#include "Engine/World.h"

FCriticalSection TelemetryBatcher::Mutex;
TArray<FAbxrTelemetryPayload> TelemetryBatcher::Payloads;
int64 TelemetryBatcher::LastCallTime = 0;
int TelemetryBatcher::Timer;
FTimerHandle TelemetryBatcher::TimerHandle;

void TelemetryBatcher::Init(const UWorld* World)
{
	Timer = GetDefault<UAbxrLibConfiguration>()->SendNextBatchWaitSeconds;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		[]
		{
			Timer--;
			if (Timer <= 0)
			{
				Send();
			}
		},
		1.0f,  // Interval
		true   // Loop
	);
}

void TelemetryBatcher::Add(FString Name, const TMap<FString, FString>& Meta)
{
	FDateTime Now = FDateTime::UtcNow(); //TODO should this be using Game Time (would need context)
	int64 UnixMillis = Now.ToUnixTimestamp() * 1000 + Now.GetMillisecond();
	FString PreciseTimestamp = FString::Printf(TEXT("%lld"), UnixMillis);

	FAbxrTelemetryPayload Payload;
	Payload.preciseTimestamp = PreciseTimestamp;
	Payload.name = Name;
	Payload.meta = Meta;

	{
		FScopeLock Lock(&Mutex);
		Payloads.Add(Payload);
		if (Payloads.Num() >= GetDefault<UAbxrLibConfiguration>()->TelemetryEntriesPerSendAttempt)
		{
			// Set timer to 0 so it triggers a send
			Timer = 0;
		}
	}
}

void TelemetryBatcher::Send()
{
	const int64 UnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
	if (UnixSeconds - LastCallTime < MaxCallFrequencySeconds) return;
		
	LastCallTime = UnixSeconds;
	Timer = GetDefault<UAbxrLibConfiguration>()->SendNextBatchWaitSeconds; // reset timer
	if (!Authentication::Authenticated()) return;
	
	{
		FScopeLock Lock(&Mutex);
		if (Payloads.Num() == 0) return;
	}

	TArray<FAbxrTelemetryPayload> TelemetriesToSend;
	{
		FScopeLock Lock(&Mutex);
		TelemetriesToSend = MoveTemp(Payloads);
	}

	FAbxrTelemetryPayloadWrapper Wrapper;
	Wrapper.data = TelemetriesToSend;

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(FAbxrTelemetryPayloadWrapper::StaticStruct(), &Wrapper, Json, 0, 0, 0, nullptr, false);

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Utils::CombineUrl(GetDefault<UAbxrLibConfiguration>()->RestUrl, TEXT("/v1/collect/telemetry")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	Authentication::SetAuthHeaders(Request, Json);

	Request->OnProcessRequestComplete().BindLambda([TelemetriesToSend](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Telemetry POST Request failed : %s"), *Response->GetContentAsString());
			Timer = GetDefault<UAbxrLibConfiguration>()->SendRetryIntervalSeconds;
			{
				FScopeLock Lock(&Mutex);
				Payloads.Insert(TelemetriesToSend, 0);
			}
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("AbxrLib - Telemetry POST Request successful: %s"), *Response->GetContentAsString());
	});
	
	Request->ProcessRequest();
}
