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
    static void SendEvent(const FString& Endpoint, const FString& EventName, const FString& PayloadJson);
};
