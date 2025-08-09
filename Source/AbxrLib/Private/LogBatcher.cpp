#include "LogBatcher.h"
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
#include "Engine/World.h"

static FCriticalSection Mutex;
static TArray<FAbxrLogPayload> Payloads;
static double LastCallTime = 0;
static constexpr double MaxCallFrequencySeconds = 1;
static int Timer;

void LogBatcher::Init(const UWorld* World)
{
	static FTimerHandle TimerHandle;
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
		if (Payloads.Num() >= 3) // TODO get from Configuration.Instance.logsPerSendAttempt
		{
			// Set timer to 0 so it triggers a send
			Timer = 0;
		}
	}
}

void LogBatcher::Send()
{
	if (FPlatformTime::Seconds() - LastCallTime < MaxCallFrequencySeconds) return;
		
	LastCallTime = FPlatformTime::Seconds();
	Timer = 10; // reset timer  TODO Configuration.Instance.sendNextBatchWaitSeconds
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
	Request->SetURL(TEXT("https://lib-backend.xrdm.app/v1/collect/log"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	Authentication::SetAuthHeaders(Request, Json);

	Request->OnProcessRequestComplete().BindLambda([LogsToSend](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
	{
		if (!bWasSuccessful || !Response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("AbxrLib - Log POST Request failed : %s"), *Response->GetContentAsString());
			Timer = 10;// TODO Configuration.Instance.sendRetryIntervalSeconds;
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
