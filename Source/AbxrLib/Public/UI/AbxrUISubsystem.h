#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbxrUISubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ShowKeyboardUI();

private:
	UFUNCTION()
	void HandlePopupClicked(const FText& ButtonText);
};
