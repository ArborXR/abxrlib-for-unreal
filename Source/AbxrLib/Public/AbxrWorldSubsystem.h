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

private:
	bool bWorldReady = false;
};
