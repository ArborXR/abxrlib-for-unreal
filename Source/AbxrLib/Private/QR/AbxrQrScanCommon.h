#pragma once

#include "CoreMinimal.h"

namespace AbxrQrScanCommon
{
	bool TryExtractPinFromQrPayload(const FString& ScanResult, FString& OutPin);
	bool IsLikelyPicoDevice();
}
