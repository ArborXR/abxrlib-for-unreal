#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbxrWidget.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubmitButtonClicked, const FString&, CurrentInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScanQRButtonClicked);

UCLASS()
class ABXRLIB_API UAbxrWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AppendString(const FString& Input);

	UFUNCTION(BlueprintCallable)
	void Backspace();
	
	UFUNCTION(BlueprintCallable)
	void SubmitInput() const { OnSubmitButtonClicked.Broadcast(CurrentInput); }
	
	UFUNCTION(BlueprintCallable)
	void ScanQR() const { OnScanQRButtonClicked.Broadcast(); }
	
	FOnSubmitButtonClicked OnSubmitButtonClicked;
	FOnScanQRButtonClicked OnScanQRButtonClicked;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Abxr|Poll")
	void InitializePoll(const TArray<FText>& Responses);
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InputTextBlock;
	
private:
	FString CurrentInput;

	void RefreshInputText() const;
};
