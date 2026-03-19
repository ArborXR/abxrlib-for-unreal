#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbxrWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubmitButtonClicked, const FText&, InputText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScanQRButtonClicked);

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
	
	FOnSubmitButtonClicked OnSubmitButtonClicked;
	FOnScanQRButtonClicked OnScanQRButtonClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InputText;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|Poll")
	void InitializePoll(const TArray<FText>& Responses);
};
