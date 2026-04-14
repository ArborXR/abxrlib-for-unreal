#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbxrQrScannerBase.generated.h"

class UAbxrQrScanWidget;
class UGameInstance;
class UTexture2D;

DECLARE_DELEGATE_TwoParams(FAbxrQrScanFinished, const FString& /*RawPayload*/, bool /*bCancelled*/);

UCLASS(Abstract)
class ABXRLIB_API UAbxrQrScannerBase : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UGameInstance* InGameInstance);

	bool IsAvailable() const { return bIsAvailable; }
	bool IsScanning() const { return bIsScanning; }
	bool IsInitializing() const { return bIsInitializing; }

	bool BeginScan(UAbxrQrScanWidget* InWidget, FAbxrQrScanFinished InCompletionCallback);
	void CancelScan();
	void Tick(float DeltaTime);

protected:
	virtual bool QueryAvailability() const PURE_VIRTUAL(UAbxrQrScannerBase::QueryAvailability, return false;);
	virtual bool StartPlatformScan();
	virtual void StopPlatformScan();
	virtual void TickPlatformScan(float DeltaTime);
	virtual bool ConsumeLatestPayload(FString& OutRawPayload);
	virtual UTexture2D* GetPreviewTexture() const;

	void FinishScan(const FString& RawPayload, bool bCancelled);
	void SetStatus(const FString& StatusText) const;

protected:
	UPROPERTY()
	TObjectPtr<UGameInstance> GameInstance = nullptr;

	TWeakObjectPtr<UAbxrQrScanWidget> ActiveWidget;
	FAbxrQrScanFinished CompletionCallback;

	bool bIsAvailable = false;
	bool bIsScanning = false;
	bool bIsInitializing = false;
	bool bInputEnabled = false;
	float InputGateRemaining = 0.0f;
};
