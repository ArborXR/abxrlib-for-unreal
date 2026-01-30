#include "AbxrDataService.h"
#include "Services/Config/AbxrSettings.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Util/AbxrUtil.h"
#include "Interfaces/IHttpResponse.h"
#include "HAL/PlatformTime.h"
#include "Types/AbxrLog.h"

bool FAbxrDataService::Tick(float /*DeltaTime*/)
{
	const double Now = FPlatformTime::Seconds();
	if (Now >= NextAt) Send();
	return true;
}

void FAbxrDataService::Start()
{
	if (bStarted) return;
	NextAt = FPlatformTime::Seconds() + GetDefault<UAbxrSettings>()->SendNextBatchWaitSeconds;
	Ticker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FAbxrDataService::Tick), 0.25f);
	bStarted = true;
}

void FAbxrDataService::Stop()
{
	if (!bStarted) return;
	bStarted = false;
	if (Ticker.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(Ticker);
		Ticker.Reset();
	}
}

static FString MakePreciseTimestamp()
{
	const FDateTime Now = FDateTime::UtcNow();
	const int64 UnixMillis = Now.ToUnixTimestamp() * 1000 + Now.GetMillisecond();
	return FString::Printf(TEXT("%lld"), UnixMillis);
}

void FAbxrDataService::AddEvent(const FString& Name, const TMap<FString, FString>& Meta)
{
	FAbxrEventPayload Payload;
	Payload.preciseTimestamp = MakePreciseTimestamp();
	Payload.name = Name;
	Payload.meta = Meta;

	FScopeLock Lock(&Mutex);
	EventPayloads.Add(Payload);
	if (EventPayloads.Num() + TelemetryPayloads.Num() + LogPayloads.Num() >=
		GetDefault<UAbxrSettings>()->DataEntriesPerSendAttempt)
	{
		NextAt = FPlatformTime::Seconds();
	}
}

void FAbxrDataService::AddTelemetry(const FString& Name, const TMap<FString, FString>& Meta)
{
	FAbxrTelemetryPayload Payload;
	Payload.preciseTimestamp = MakePreciseTimestamp();
	Payload.name = Name;
	Payload.meta = Meta;

	FScopeLock Lock(&Mutex);
	TelemetryPayloads.Add(Payload);
	if (EventPayloads.Num() + TelemetryPayloads.Num() + LogPayloads.Num() >=
		GetDefault<UAbxrSettings>()->DataEntriesPerSendAttempt)
	{
		NextAt = FPlatformTime::Seconds();
	}
}

void FAbxrDataService::AddLog(const FString& Level, const FString& Text, const TMap<FString, FString>& Meta)
{
	FAbxrLogPayload Payload;
	Payload.preciseTimestamp = MakePreciseTimestamp();
	Payload.logLevel = Level;
	Payload.text = Text;
	Payload.meta = Meta;

	FScopeLock Lock(&Mutex);
	LogPayloads.Add(Payload);
	if (EventPayloads.Num() + TelemetryPayloads.Num() + LogPayloads.Num() >=
		GetDefault<UAbxrSettings>()->DataEntriesPerSendAttempt)
	{
		NextAt = FPlatformTime::Seconds();
	}
}

void FAbxrDataService::Send()
{
	const int64 UnixSeconds = FDateTime::UtcNow().ToUnixTimestamp();
	if (UnixSeconds - LastCallTime < GetDefault<UAbxrSettings>()->MaxCallFrequencySeconds) return;
	LastCallTime = UnixSeconds;
	NextAt = FPlatformTime::Seconds() + GetDefault<UAbxrSettings>()->SendNextBatchWaitSeconds;
	if (!AuthService.Authenticated()) return;

	TArray<FAbxrEventPayload> EventsToSend;
	TArray<FAbxrTelemetryPayload> TelemetriesToSend;
	TArray<FAbxrLogPayload> LogsToSend;
	{
		FScopeLock Lock(&Mutex);
		if (EventPayloads.Num() == 0 && TelemetryPayloads.Num() == 0 && LogPayloads.Num() == 0) return;
		EventsToSend = MoveTemp(EventPayloads);
		TelemetriesToSend = MoveTemp(TelemetryPayloads);
		LogsToSend = MoveTemp(LogPayloads);
	}

	FAbxrDataPayloadWrapper Wrapper;
	Wrapper.event = EventsToSend;
	Wrapper.telemetry = TelemetriesToSend;
	Wrapper.basicLog = LogsToSend;

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(FAbxrDataPayloadWrapper::StaticStruct(), &Wrapper, Json, 0, 0, 0, nullptr, false);

	const FString Url = AbxrUtil::CombineUrl(GetDefault<UAbxrSettings>()->RestUrl, TEXT("/v1/collect/data"));
	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(Json);
	AuthService.SetAuthHeaders(Request, Json);

	Request->OnProcessRequestComplete().BindLambda(
		[EventsToSend, TelemetriesToSend, LogsToSend, DataPtr = AsWeak()](FHttpRequestPtr, const FHttpResponsePtr& Response, const bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				const TSharedPtr<FAbxrDataService> Self = DataPtr.Pin();
				if (!Self) return;
				UE_LOG(LogAbxrLib, Error, TEXT("Data POST failed: %s"), *Response->GetContentAsString());
				{
					FScopeLock Lock(&Self->Mutex);
					Self->EventPayloads.Insert(EventsToSend, 0);
					Self->TelemetryPayloads.Insert(TelemetriesToSend, 0);
					Self->LogPayloads.Insert(LogsToSend, 0);
				}
				Self->NextAt = FPlatformTime::Seconds() + GetDefault<UAbxrSettings>()->SendRetryIntervalSeconds;
				return;
			}
			UE_LOG(LogAbxrLib, Log, TEXT("Data POST successful: %s"), *Response->GetContentAsString());
		});
	Request->ProcessRequest();
}
