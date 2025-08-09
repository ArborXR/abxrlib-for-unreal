#include "LogBatcher.h"
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

FCriticalSection LogBatcher::Mutex;
TArray<FAbxrLogPayload> LogBatcher::Payloads;
int64 LogBatcher::LastCallTime = 0;
int LogBatcher::Timer;
FTimerHandle LogBatcher::TimerHandle;

void LogBatcher::Init(const UWorld* World)
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

void LogBatcher::Add(FString Level, FString Text, const TMap<FString, FString>& Meta)
{
	FDateTime Now = FDateTime::UtcNow(); //TODO should this be using Game Time (would need context)
	int64 UnixMillis = Now.ToUnixTimestamp() * 1000 + Now.GetMillisecond();
	FString PreciseTimestamp = FString::Printf(TEXT("%lld"), UnixMillis);

	FAbxrLogPayload Payload;
	Payload.preciseTimestamp = PreciseTimestamp;
	Payload.logLevel = Level;
	Payload.text = Text;
	Payload.meta = Meta;

	{
		FScopeLock Lock(&Mutex);
		Payloads.Add(Payload);
		if (Payloads.Num() >= GetDefault<UAbxrLibConfiguration>()->LogsPerSendAttempt)
		{
			// Set timer to 0 so it triggers a send
			Timer = 0;
		}
	}
}

void LogBatcher::Send()
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

	TArray<FAbxrLogPayload> LogsToSend;
	{
		FScopeLock Lock(&Mutex);
		LogsToSend = MoveTemp(Payloads);
	}

	FAbxrLogPayloadWrapper Wrapper;
	Wrapper.data = LogsToSend;

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(FAbxrLogPayloadWrapper::StaticStruct(), &Wrapper, Json, 0, 0, 0, nullptr, false);

	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Utils::CombineUrl(GetDefault<UAbxrLibConfiguration>()->RestUrl, TEXT("/v1/collect/log")));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	Authentication::SetAuthHeaders(Request, Json);

	Request->OnProcessRequestComplete().BindLambda([LogsToSend](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Log POST Request failed : %s"), *Response->GetContentAsString());
			Timer = GetDefault<UAbxrLibConfiguration>()->SendRetryIntervalSeconds;
			{
				FScopeLock Lock(&Mutex);
				Payloads.Insert(LogsToSend, 0);
			}
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("AbxrLib - Log POST Request successful"));
	});
	
	Request->ProcessRequest();
}
