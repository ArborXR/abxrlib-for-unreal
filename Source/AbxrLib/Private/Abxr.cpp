#include "Abxr.h"
#include "Authentication.h"
#include "EventBatcher.h"
#include "LogBatcher.h"
#include "TelemetryBatcher.h"

void UAbxr::Authenticate()
{
    Authentication::Authenticate();
}

void UAbxr::LogDebug(const FString& text, const TMap<FString, FString>& meta)
{
    LogBatcher::Add("debug", text, meta);
}

void UAbxr::LogInfo(const FString& text, const TMap<FString, FString>& meta)
{
    LogBatcher::Add("info", text, meta);
}

void UAbxr::LogWarn(const FString& text, const TMap<FString, FString>& meta)
{
    LogBatcher::Add("warn", text, meta);
}

void UAbxr::LogError(const FString& text, const TMap<FString, FString>& meta)
{
    LogBatcher::Add("error", text, meta);
}

void UAbxr::LogCritical(const FString& text, const TMap<FString, FString>& meta)
{
    LogBatcher::Add("critical", text, meta);
}

void UAbxr::Event(const FString& Name, const TMap<FString, FString>& Meta)
{
	EventBatcher::Add(Name, Meta);
}

void UAbxr::TelemetryEntry(const FString& name, const TMap<FString, FString>& meta)
{
    TelemetryBatcher::Add(name, meta);
}
