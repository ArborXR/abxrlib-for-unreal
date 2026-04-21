#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AbxrWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubmitButtonClicked, const FText&, InputText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScanQRButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelQRButtonClicked);

UCLASS()
class ABXRLIB_API UAbxrWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void SubmitInput() const { OnSubmitButtonClicked.Broadcast(InputText); }
	
	UFUNCTION(BlueprintCallable)
	void ScanQR() const { OnScanQRButtonClicked.Broadcast(); }

	UFUNCTION(BlueprintCallable)
	void CancelQR() const { OnCancelQRButtonClicked.Broadcast(); }
	
	FOnSubmitButtonClicked OnSubmitButtonClicked;
	FOnScanQRButtonClicked OnScanQRButtonClicked;
	FOnCancelQRButtonClicked OnCancelQRButtonClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InputText;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UImage* QRPreviewImage = nullptr;
	
	UFUNCTION(BlueprintCallable)
	void SetQRPreviewTexture(UTexture2D* Texture);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|Poll")
	void InitializePoll(const TArray<FText>& Responses);
};
