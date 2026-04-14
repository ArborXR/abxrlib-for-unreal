#pragma once

#include "CoreMinimal.h"
#include "QR/AbxrQrScannerBase.h"
#include "AbxrPicoQrScanner.generated.h"

class UTexture2D;

UCLASS()
class ABXRLIB_API UAbxrPicoQrScanner : public UAbxrQrScannerBase
{
	GENERATED_BODY()

public:
	static bool RegisterJavaBridge();

protected:
	virtual bool QueryAvailability() const override;
	virtual bool StartPlatformScan() override;
	virtual void StopPlatformScan() override;
	virtual void TickPlatformScan(float DeltaTime) override;
	virtual bool ConsumeLatestPayload(FString& OutRawPayload) override;
	virtual UTexture2D* GetPreviewTexture() const override;

private:
	enum class EBridgeStatus : int32
	{
		Starting = 0,
		Scanning = 1,
		PermissionDenied = 2,
		Error = 3,
	};

	bool CallJavaVoidMethod(const ANSICHAR* MethodName) const;
	void ConsumeBridgeUpdates();
	void UpdatePreviewTextureFromBGRA(const TArray<uint8>& InBgraBytes, int32 Width, int32 Height);
	void FailAndCancel(const FString& Reason);

private:
	UPROPERTY()
	TObjectPtr<UTexture2D> PreviewTexture = nullptr;

	FString PendingPayload;
	float TimeSinceStartSeconds = 0.0f;
	bool bHasEnteredScanningState = false;
	bool bPendingFailure = false;
	FString PendingFailureReason;

	static constexpr float StartupTimeoutSeconds = 8.0f;
};
