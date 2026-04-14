#include "QR/AbxrPicoQrScanner.h"

#include "QR/AbxrQrScanCommon.h"
#include "Types/AbxrLog.h"

bool UAbxrPicoQrScanner::QueryAvailability() const
{
	return AbxrQrScanCommon::IsLikelyPicoDevice();
}

bool UAbxrPicoQrScanner::StartPlatformScan()
{
	TimeSinceStartSeconds = 0.0f;
	PendingPayload.Reset();

	if (!Super::StartPlatformScan()) return false;

	SetStatus(TEXT("QR camera backend hook ready - move your PICO scan code into UAbxrPicoQrScanner.cpp"));
	UE_LOG(LogAbxrLib, Log, TEXT("PICO QR session started. Backend seam is ready in UAbxrPicoQrScanner."));
	return true;
}

void UAbxrPicoQrScanner::StopPlatformScan()
{
	PendingPayload.Reset();
	PreviewTexture = nullptr;
	TimeSinceStartSeconds = 0.0f;
}

void UAbxrPicoQrScanner::TickPlatformScan(float DeltaTime)
{
	TickBackendPlaceholder(DeltaTime);
}

bool UAbxrPicoQrScanner::ConsumeLatestPayload(FString& OutRawPayload)
{
	if (PendingPayload.IsEmpty()) return false;
	OutRawPayload = MoveTemp(PendingPayload);
	PendingPayload.Reset();
	return true;
}

UTexture2D* UAbxrPicoQrScanner::GetPreviewTexture() const
{
	return PreviewTexture;
}

void UAbxrPicoQrScanner::TickBackendPlaceholder(float DeltaTime)
{
	TimeSinceStartSeconds += DeltaTime;

	// This is the one file you should replace with your platform backend.
	// A good direct port of your Unity Pico reader is:
	//   - async open camera
	//   - keep a preview texture updated every frame
	//   - decode on a 0.5 second cadence
	//   - when you get a matching ABXR payload, assign PendingPayload = RawText;
	//
	// For the current patch we intentionally do not fake success.
	// The QR popup stays open and can be cancelled, which lets you verify the UI,
	// popup swapping, and auth handoff plumbing before wiring camera access.
}
