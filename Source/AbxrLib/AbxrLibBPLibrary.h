#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbxrLibBPLibrary.generated.h"

UCLASS()
class ABXRLIB_API UAbxrLibBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AbxrLib")
    static void SendEvent(const FString& Endpoint, const FString& EventName, const FString& PayloadJson);
};
