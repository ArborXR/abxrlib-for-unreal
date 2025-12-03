#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbxrGameInstanceSubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void OnPostLoadMapWithWorld(UWorld* LoadedWorld);
	void StartAuthAfterDelay();
	
	FTimerHandle AuthenticationTimerHandle;
	FDelegateHandle PostLoadMapHandle;
	bool bInitialized = false;
};
