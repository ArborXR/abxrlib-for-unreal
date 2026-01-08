#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "AbxrWorldSubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void ShowKeyboardUI(float Distance = 400.f, float VerticalOffset = 0.f);

private:
	UFUNCTION()
	void HandlePopupClicked(const FText& ButtonText);

	bool bWorldReady = false;
};
