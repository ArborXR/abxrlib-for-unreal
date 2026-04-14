#include "QR/AbxrQrScannerBase.h"

#include "QR/AbxrQrScanWidget.h"
#include "Services/Config/AbxrSettings.h"
#include "Engine/GameInstance.h"

void UAbxrQrScannerBase::Initialize(UGameInstance* InGameInstance)
{
	GameInstance = InGameInstance;
	bIsAvailable = QueryAvailability();
}

bool UAbxrQrScannerBase::BeginScan(UAbxrQrScanWidget* InWidget, FAbxrQrScanFinished InCompletionCallback)
{
	if (!bIsAvailable || !InWidget || bIsScanning || bIsInitializing) return false;

	ActiveWidget = InWidget;
	CompletionCallback = MoveTemp(InCompletionCallback);
	bIsInitializing = true;
	bIsScanning = false;
	bInputEnabled = false;
	InputGateRemaining = FMath::Max(0.0f, GetDefault<UAbxrSettings>()->QrPopupInputDelaySeconds);

	InWidget->SetScannerAvailable(true);
	InWidget->SetScanInputEnabled(InputGateRemaining <= 0.0f);
	InWidget->SetPreviewTexture(nullptr);
	InWidget->SetStatusText(FText::FromString(TEXT("Starting camera...")));

	if (!StartPlatformScan())
	{
		FinishScan(TEXT(""), true);
		return false;
	}

	return true;
}

void UAbxrQrScannerBase::CancelScan()
{
	if (!bIsScanning && !bIsInitializing) return;
	FinishScan(TEXT(""), true);
}

void UAbxrQrScannerBase::Tick(float DeltaTime)
{
	if (!bIsScanning && !bIsInitializing) return;

	if (!bInputEnabled)
	{
		InputGateRemaining -= DeltaTime;
		if (InputGateRemaining <= 0.0f)
		{
			bInputEnabled = true;
			if (UAbxrQrScanWidget* Widget = ActiveWidget.Get())
			{
				Widget->SetScanInputEnabled(true);
			}
		}
	}

	TickPlatformScan(DeltaTime);

	if (UAbxrQrScanWidget* Widget = ActiveWidget.Get())
	{
		if (UTexture2D* PreviewTexture = GetPreviewTexture())
		{
			Widget->SetPreviewTexture(PreviewTexture);
		}
	}

	FString RawPayload;
	if (ConsumeLatestPayload(RawPayload))
	{
		FinishScan(RawPayload, false);
	}
}

bool UAbxrQrScannerBase::StartPlatformScan()
{
	bIsInitializing = false;
	bIsScanning = true;
	return true;
}

void UAbxrQrScannerBase::StopPlatformScan()
{
}

void UAbxrQrScannerBase::TickPlatformScan(float DeltaTime)
{
}

bool UAbxrQrScannerBase::ConsumeLatestPayload(FString& OutRawPayload)
{
	return false;
}

UTexture2D* UAbxrQrScannerBase::GetPreviewTexture() const
{
	return nullptr;
}

void UAbxrQrScannerBase::FinishScan(const FString& RawPayload, bool bCancelled)
{
	StopPlatformScan();
	bIsScanning = false;
	bIsInitializing = false;
	bInputEnabled = false;
	InputGateRemaining = 0.0f;

	FAbxrQrScanFinished Callback = MoveTemp(CompletionCallback);
	CompletionCallback.Unbind();
	ActiveWidget.Reset();

	if (Callback.IsBound())
	{
		Callback.Execute(RawPayload, bCancelled);
	}
}

void UAbxrQrScannerBase::SetStatus(const FString& StatusText) const
{
	if (UAbxrQrScanWidget* Widget = ActiveWidget.Get())
	{
		Widget->SetStatusText(FText::FromString(StatusText));
	}
}
