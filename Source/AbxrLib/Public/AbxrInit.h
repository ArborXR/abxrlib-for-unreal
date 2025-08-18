#pragma once
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "AbxrInit.generated.h"

UCLASS()
class UAbxrInit : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

private:
	FTimerHandle MyRepeatingTimer;
	void MyRepeatingFunction();
};
