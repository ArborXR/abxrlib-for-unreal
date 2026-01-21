#pragma once
#include "CoreMinimal.h"

class AbxrUtil
{
public:
	static FString ComputeSHA256(const FString& Input);
	static uint32 ComputeCRC32(const FString& Input);
	static FString CombineUrl(const FString& Base, const FString& Path);
	static bool IsUuidFormat(const FString& Input);
	static bool IsValidUrl(const FString& InUrl);
};
