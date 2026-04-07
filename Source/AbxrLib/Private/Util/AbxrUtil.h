#pragma once
#include "CoreMinimal.h"

class FAbxrUtil
{
public:
	static FString ComputeSHA256(const FString& Input);
	static uint32 ComputeCRC32(const FString& Input);
	static FString CombineUrl(const FString& Base, const FString& Path);
	static bool IsValidUrl(const FString& InUrl);
	static bool IsPackageInstalled(const FString& PackageName);
	static FString BuildOrgToken(const FString& OrgId, const FString& Fingerprint);
	
private:
	static FString Base64UrlEncode(const TArray<uint8>& Bytes);
};
