#pragma once

#include "Engine/GameInstance.h"
#include "AbxrInit.generated.h"

UCLASS()
class ABXRLIB_API UAbxrInit : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
