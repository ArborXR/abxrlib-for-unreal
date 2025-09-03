#pragma once

#include "Blueprint/UserWidget.h"
#include "PinPadWidget.generated.h"

// Native (non-UObject) multicast delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPinAccepted_Native, const FString& /*Pin*/);

/**
 * Programmatically generated PIN pad widget.
 * Creates a 3x4 grid with digits 0-9, delete, and submit buttons.
 * Pure C++ API (no Blueprint). Works in PIE and packaged builds.
 */
UCLASS()
class ABXRLIB_API UPinPadWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Create and show the PIN pad dialog
    static UPinPadWidget* ShowPinPad(UWorld* World, const FText& Message, int32 ZOrder = 10000);

    // Callbacks (bind with AddLambda / AddRaw / AddUObject, etc.)
    FOnPinAccepted_Native OnPinAccepted;
    
    void ClosePinPad();

    // Get current PIN (masked as dots for display)
    FString GetMaskedPin() const;

    // Get actual PIN value
    FString GetActualPin() const { return CurrentPin; }

protected:
    // UUserWidget
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    void BuildWidgetTree(const FText& Message);
    void CaptureFocusSoon();
    void ApplyUIInputMode();
    void RestorePreviousInputMode() const;
    
    // Button creation helpers
    class UButton* CreateDigitButton(const FString& Digit);
    class UButton* CreateActionButton(const FString& Label, const FLinearColor& Color = FLinearColor::White);
    
    // Event handlers
    UFUNCTION()
    void HandleDigit0Clicked() { AddDigit(TEXT("0")); }
    
    UFUNCTION()
    void HandleDigit1Clicked() { AddDigit(TEXT("1")); }
    
    UFUNCTION()
    void HandleDigit2Clicked() { AddDigit(TEXT("2")); }
    
    UFUNCTION()
    void HandleDigit3Clicked() { AddDigit(TEXT("3")); }
    
    UFUNCTION()
    void HandleDigit4Clicked() { AddDigit(TEXT("4")); }
    
    UFUNCTION()
    void HandleDigit5Clicked() { AddDigit(TEXT("5")); }
    
    UFUNCTION()
    void HandleDigit6Clicked() { AddDigit(TEXT("6")); }
    
    UFUNCTION()
    void HandleDigit7Clicked() { AddDigit(TEXT("7")); }
    
    UFUNCTION()
    void HandleDigit8Clicked() { AddDigit(TEXT("8")); }
    
    UFUNCTION()
    void HandleDigit9Clicked() { AddDigit(TEXT("9")); }
    
    UFUNCTION()
    void HandleDeleteClicked();
    
    UFUNCTION()
    void HandleSubmitClicked();
    
    void UpdatePinDisplay();
    void AddDigit(const FString& Digit);
    void RemoveLastDigit();

    // UI Components
    UPROPERTY(Transient) TObjectPtr<class UTextBlock> PinDisplay = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UButton> DeleteButton = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UButton> SubmitButton = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UBorder> OutlineBorder = nullptr;
    UPROPERTY(Transient) TObjectPtr<class UBorder> Card = nullptr;
    
    // Digit buttons (0-9)
    UPROPERTY(Transient) TArray<TObjectPtr<class UButton>> DigitButtons;

    // Input mode management
    TWeakObjectPtr<APlayerController> OwningPC;
    bool bPrevMouseVisible = false;
    bool bPrevEnableClickEvents = false;
    bool bPrevEnableMouseOverEvents = false;

    // PIN data
    FString CurrentPin;
    static constexpr int32 MaxPinLength = 6; // Configurable max PIN length
};
