#pragma once

#include "CoreMinimal.h"

class Utils
{
public:
	static FString ComputeSHA256(const FString& Input);
	static uint32 ComputeCRC32(const FString& Input);
};
