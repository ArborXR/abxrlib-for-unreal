#include "QR/AbxrQrScannerCoordinator.h"

#include "QR/AbxrPicoQrScanner.h"
#include "QR/AbxrQrScanWidget.h"
#include "QR/AbxrQrScannerBase.h"

void UAbxrQrScannerCoordinator::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbxrPicoQrScanner* PicoScanner = NewObject<UAbxrPicoQrScanner>(this);
	PicoScanner->Initialize(GetGameInstance());
	ActiveScanner = PicoScanner;
}

void UAbxrQrScannerCoordinator::Deinitialize()
{
	CancelScan();
	ActiveScanner = nullptr;
	OnFinishedCallback = nullptr;
	Super::Deinitialize();
}

void UAbxrQrScannerCoordinator::Tick(float DeltaTime)
{
	if (ActiveScanner)
	{
		ActiveScanner->Tick(DeltaTime);
	}
}

bool UAbxrQrScannerCoordinator::IsTickable() const
{
	return ActiveScanner && (ActiveScanner->IsScanning() || ActiveScanner->IsInitializing());
}

TStatId UAbxrQrScannerCoordinator::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAbxrQrScannerCoordinator, STATGROUP_Tickables);
}

bool UAbxrQrScannerCoordinator::IsQrScanningAvailable() const
{
	return ActiveScanner && ActiveScanner->IsAvailable();
}

bool UAbxrQrScannerCoordinator::IsScanActive() const
{
	return ActiveScanner && (ActiveScanner->IsScanning() || ActiveScanner->IsInitializing());
}

bool UAbxrQrScannerCoordinator::BeginScan(UAbxrQrScanWidget* Widget, TFunction<void(const FString&, bool)> OnFinished)
{
	if (!ActiveScanner || !Widget || !ActiveScanner->IsAvailable() || IsScanActive()) return false;

	OnFinishedCallback = MoveTemp(OnFinished);
	return ActiveScanner->BeginScan(Widget, FAbxrQrScanFinished::CreateWeakLambda(this,
		[this](const FString& RawPayload, bool bCancelled)
		{
			TFunction<void(const FString&, bool)> Callback = MoveTemp(OnFinishedCallback);
			OnFinishedCallback = nullptr;
			if (Callback)
			{
				Callback(RawPayload, bCancelled);
			}
		}));
}

void UAbxrQrScannerCoordinator::CancelScan()
{
	if (ActiveScanner)
	{
		ActiveScanner->CancelScan();
	}
}
