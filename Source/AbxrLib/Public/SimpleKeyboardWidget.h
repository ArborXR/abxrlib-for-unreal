#pragma once

#include "Blueprint/UserWidget.h"
#include "SimpleKeyboardWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DELEGATE_OneParam(FOnKeyboardDone, const FString&);

UCLASS()
class USimpleKeyboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Call this from C++ to get the result when "Done" is pressed
	FOnKeyboardDone OnKeyboardDone;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;

	// For the "simplest" keyboard, we just do A, B, Backspace, Done
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_A;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_B;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Backspace;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Done;

private:
	FString CurrentText;

	UFUNCTION()
	void OnA();

	UFUNCTION()
	void OnB();

	UFUNCTION()
	void OnBackspace();

	UFUNCTION()
	void OnDone();

	void RefreshDisplay();
};
