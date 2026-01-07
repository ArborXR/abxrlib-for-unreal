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
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	UFUNCTION()
	void HandlePopupClicked(const FText& ButtonText);

	bool bWorldReady = false;
};
