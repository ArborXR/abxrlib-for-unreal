#pragma once

#include "CoreMinimal.h"
#include "UI/AbxrWidget.h"
#include "AbxrQrScanWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQrCancelClicked);

class UTexture2D;

UCLASS(Abstract)
class ABXRLIB_API UAbxrQrScanWidget : public UAbxrWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Abxr|QR")
	FOnQrCancelClicked OnCancelClicked;

	UFUNCTION(BlueprintCallable, Category="Abxr|QR")
	void CancelScan() { OnCancelClicked.Broadcast(); }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|QR")
	void SetStatusText(const FText& InStatus);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|QR")
	void SetPreviewTexture(UTexture2D* InTexture);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|QR")
	void SetScanInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|QR")
	void SetScannerAvailable(bool bAvailable);
};
