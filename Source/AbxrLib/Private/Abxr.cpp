#include "Abxr.h"
#include "Authentication.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"

void UAbxr::Authenticate()
{
    Authentication::Authenticate();
}

void UAbxr::LogDebug(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("debug", Text, Meta);
}

void UAbxr::LogInfo(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("info", Text, Meta);
}

void UAbxr::LogWarn(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("warn", Text, Meta);
}

void UAbxr::LogError(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("error", Text, Meta);
}

void UAbxr::LogCritical(const FString& Text, const TMap<FString, FString>& Meta)
{
    LogBatcher::Add("critical", Text, Meta);
}

void UAbxr::Event(const FString& Name, const TMap<FString, FString>& Meta)
{
	EventBatcher::Add(Name, Meta);
}

void UAbxr::TelemetryEntry(const FString& Name, const TMap<FString, FString>& Meta)
{
    TelemetryBatcher::Add(Name, Meta);
}
