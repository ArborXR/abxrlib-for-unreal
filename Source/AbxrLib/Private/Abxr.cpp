#include "Abxr.h"
#include "Authentication.h"
#include "EventBatcher.h"

void UAbxr::Authenticate()
{
    Authentication::Authenticate();
}

void UAbxr::LogDebug(const FString& text, const TMap<FString, FString>& meta)
{
    
}

void UAbxr::LogInfo(const FString& text, const TMap<FString, FString>& meta)
{
    
}

void UAbxr::LogWarn(const FString& text, const TMap<FString, FString>& meta)
{
    
}

void UAbxr::LogError(const FString& text, const TMap<FString, FString>& meta)
{
    
}

void UAbxr::LogCritical(const FString& text, const TMap<FString, FString>& meta)
{
    
}

void UAbxr::Event(const FString& Name, const TMap<FString, FString>& Meta)
{
	EventBatcher::Add(Name, Meta);
	EventBatcher::Add("SomeOtherEvent", Meta);
	EventBatcher::Send();
}

void UAbxr::TelemetryEntry(const FString& name, const TMap<FString, FString>& meta)
{
    
}
