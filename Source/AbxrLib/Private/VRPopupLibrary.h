// VRPopupLibrary.h

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRPopupLibrary.generated.h"

UCLASS()
class ABXRLIB_API UVRPopupLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Spawns the popup actor (BP_VRPopupActor) in front of the player's view.
	UFUNCTION(BlueprintCallable, Category = "VR Popup",
			  meta = (WorldContext = "WorldContextObject"))
	static AActor* SpawnPopupButtonInFrontOfPlayer(
		UObject* WorldContextObject,
		float Distance = 150.f,
		float VerticalOffset = 0.f
	);
};
