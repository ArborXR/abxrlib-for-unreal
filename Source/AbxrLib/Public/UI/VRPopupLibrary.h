#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRPopupLibrary.generated.h"

UCLASS()
class ABXRLIB_API UVRPopupLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "VR Popup", meta = (WorldContext = "WorldContextObject"))
	static AActor* SpawnPopupButtonInFrontOfPlayer(UObject* WorldContextObject);
};
