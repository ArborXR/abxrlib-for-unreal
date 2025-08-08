#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abxr.generated.h"

UCLASS()
class ABXRLIB_API UAbxr : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Authenticate();
	
	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogDebug(const FString& text, const TMap<FString, FString>& meta);
	static void LogDebug(const FString& text) { LogDebug(text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogInfo(const FString& text, const TMap<FString, FString>& meta);
	static void LogInfo(const FString& text) { LogInfo(text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogWarn(const FString& text, const TMap<FString, FString>& meta);
	static void LogWarn(const FString& text) { LogWarn(text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogError(const FString& text, const TMap<FString, FString>& meta);
	static void LogError(const FString& text) { LogError(text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void LogCritical(const FString& text, const TMap<FString, FString>& meta);
	static void LogCritical(const FString& text) { LogCritical(text, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void Event(const FString& name, const TMap<FString, FString>& meta);
	static void Event(const FString& name) { Event(name, TMap<FString, FString>()); }

	UFUNCTION(BlueprintCallable, Category = "AbxrLib")
	static void TelemetryEntry(const FString& name, const TMap<FString, FString>& meta);
	static void TelemetryEntry(const FString& name) { TelemetryEntry(name, TMap<FString, FString>()); }
};
