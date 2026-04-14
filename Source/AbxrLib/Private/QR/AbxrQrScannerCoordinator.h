#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AbxrQrScannerCoordinator.generated.h"

class UAbxrQrScanWidget;
class UAbxrQrScannerBase;

UCLASS()
class ABXRLIB_API UAbxrQrScannerCoordinator : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	bool IsQrScanningAvailable() const;
	bool IsScanActive() const;
	bool BeginScan(UAbxrQrScanWidget* Widget, TFunction<void(const FString&, bool)> OnFinished);
	void CancelScan();

private:
	UPROPERTY()
	TObjectPtr<UAbxrQrScannerBase> ActiveScanner = nullptr;

	TFunction<void(const FString&, bool)> OnFinishedCallback;
};
