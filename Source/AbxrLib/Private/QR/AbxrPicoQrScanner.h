#pragma once

#include "CoreMinimal.h"
#include "QR/AbxrQrScannerBase.h"
#include "AbxrPicoQrScanner.generated.h"

class UTexture2D;

UCLASS()
class ABXRLIB_API UAbxrPicoQrScanner : public UAbxrQrScannerBase
{
	GENERATED_BODY()

protected:
	virtual bool QueryAvailability() const override;
	virtual bool StartPlatformScan() override;
	virtual void StopPlatformScan() override;
	virtual void TickPlatformScan(float DeltaTime) override;
	virtual bool ConsumeLatestPayload(FString& OutRawPayload) override;
	virtual UTexture2D* GetPreviewTexture() const override;

private:
	/**
	 * Port your current Unreal or Unity-equivalent PICO camera/decode backend here.
	 *
	 * Target behavior:
	 *  1. Open the PICO camera asynchronously.
	 *  2. Feed a preview UTexture2D to PreviewTexture.
	 *  3. Decode every ~0.5s and store a matching ABXR payload in PendingPayload.
	 *  4. Return that payload once from ConsumeLatestPayload().
	 *
	 * I left this seam isolated so you can drop in the device-specific logic without
	 * touching the popup/session/auth flow again.
	 */
	void TickBackendPlaceholder(float DeltaTime);

private:
	UPROPERTY()
	TObjectPtr<UTexture2D> PreviewTexture = nullptr;

	FString PendingPayload;
	float TimeSinceStartSeconds = 0.0f;
};
