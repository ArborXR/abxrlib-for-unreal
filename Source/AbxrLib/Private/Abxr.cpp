#include "Abxr.h"
#include "Authentication.h"

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

void UAbxr::Event(const FString& name, const TMap<FString, FString>& meta)
{
	UE_LOG(LogTemp, Warning, TEXT("EVENT!!!"));
}

void UAbxr::TelemetryEntry(const FString& name, const TMap<FString, FString>& meta)
{
    
}
