#pragma once

#include "Blueprint/UserWidget.h"
#include "InputDialogWidget.generated.h"

// Native (non-UObject) multicast delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialogAccepted_Native, const FString&/*, Text*/);

/**
 * Minimal UMG dialog (message + single-line text box).
 * Pure C++ API (no Blueprint). Works in PIE and packaged on Windows.
 */
UCLASS()
class ABXRLIB_API UInputDialogWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Create, add to viewport, focus the edit box, set UI-only input (restored on close).
    static UInputDialogWidget* ShowDialog(UWorld* World, const FText& Message,
        const FText& Placeholder = FText(), bool bStartWithExistingText = false,
        const FString& ExistingText = FString(), int32 ZOrder = 10000);

    // Callbacks (bind with AddLambda / AddRaw / AddUObject, etc.)
    FOnDialogAccepted_Native OnAccepted;
    
    void CloseDialog();

    // Set current text
    void SetText(const FString& NewText) const;

protected:
    // UUserWidget
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    void BuildWidgetTree(const FText& Message, const FText& Placeholder, bool bStartWithExistingText, const FString& ExistingText);
    void CaptureFocusSoon();
    void ApplyUIInputMode();
    void RestorePreviousInputMode() const;
    
    UPROPERTY(Transient) TObjectPtr<class UEditableTextBox> EditBox = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UButton> OkButton = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UBorder> OutlineBorder = nullptr;
    UPROPERTY(Transient) TObjectPtr<UBorder> Card = nullptr;

    TWeakObjectPtr<APlayerController> OwningPC;
    bool bPrevMouseVisible = false;
    bool bPrevEnableClickEvents = false;
    bool bPrevEnableMouseOverEvents = false;

    UFUNCTION()
    void HandleOkClicked();

    UFUNCTION()
    void HandleTextCommitted(const FText& Text, ETextCommit::Type Method);
};
