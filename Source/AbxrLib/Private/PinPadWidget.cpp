#include "PinPadWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/World.h"
#include "Styling/CoreStyle.h"
#include "TimerManager.h"
#include "Authentication.h"

UPinPadWidget* UPinPadWidget::ShowPinPad(UWorld* World, const FText& Message, const int32 ZOrder)
{
    UPinPadWidget* PinPad = CreateWidget<UPinPadWidget>(World);
    if (!PinPad) return nullptr;

    PinPad->BuildWidgetTree(Message);
    PinPad->AddToViewport(ZOrder);
    PinPad->ApplyUIInputMode();
    PinPad->CaptureFocusSoon();

    return PinPad;
}

void UPinPadWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind individual digit button events
    if (DigitButtons.Num() >= 10)
    {
        if (DigitButtons[0]) DigitButtons[0]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit1Clicked);
        if (DigitButtons[1]) DigitButtons[1]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit2Clicked);
        if (DigitButtons[2]) DigitButtons[2]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit3Clicked);
        if (DigitButtons[3]) DigitButtons[3]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit4Clicked);
        if (DigitButtons[4]) DigitButtons[4]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit5Clicked);
        if (DigitButtons[5]) DigitButtons[5]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit6Clicked);
        if (DigitButtons[6]) DigitButtons[6]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit7Clicked);
        if (DigitButtons[7]) DigitButtons[7]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit8Clicked);
        if (DigitButtons[8]) DigitButtons[8]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit9Clicked);
        if (DigitButtons[9]) DigitButtons[9]->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDigit0Clicked);
    }

    // Bind action button events
    if (DeleteButton)
    {
        DeleteButton->OnClicked.Clear();
        DeleteButton->OnClicked.AddDynamic(this, &UPinPadWidget::HandleDeleteClicked);
    }

    if (SubmitButton)
    {
        SubmitButton->OnClicked.Clear();
        SubmitButton->OnClicked.AddDynamic(this, &UPinPadWidget::HandleSubmitClicked);
    }
}

FReply UPinPadWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey Key = InKeyEvent.GetKey();
    
    // Handle number keys
    if (Key >= EKeys::Zero && Key <= EKeys::Nine)
    {
        int32 Digit = Key.GetFName().GetNumber() - EKeys::Zero.GetFName().GetNumber();
        AddDigit(FString::FromInt(Digit));
        return FReply::Handled();
    }
    
    // Handle numpad keys
    if (Key >= EKeys::NumPadZero && Key <= EKeys::NumPadNine)
    {
        int32 Digit = Key.GetFName().GetNumber() - EKeys::NumPadZero.GetFName().GetNumber();
        AddDigit(FString::FromInt(Digit));
        return FReply::Handled();
    }
    
    // Handle backspace/delete
    if (Key == EKeys::BackSpace || Key == EKeys::Delete)
    {
        RemoveLastDigit();
        return FReply::Handled();
    }
    
    // Handle enter/submit
    if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
    {
        if (CurrentPin.Len() > 0)
        {
            ClosePinPad();
        }
        return FReply::Handled();
    }
    
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPinPadWidget::ClosePinPad()
{
    OnPinAccepted.Broadcast(CurrentPin);
    RemoveFromParent();
    RestorePreviousInputMode();
}

FString UPinPadWidget::GetMaskedPin() const
{
    FString MaskedPin;
    for (int32 i = 0; i < CurrentPin.Len(); ++i)
    {
        MaskedPin += TEXT("•");
    }
    return MaskedPin;
}

void UPinPadWidget::BuildWidgetTree(const FText& Message)
{
    constexpr float CardWidth = 400.f;
    constexpr float CardMaxH = 600.f;
    constexpr float Pad = 20.f;
    constexpr float ButtonSize = 60.f;
    constexpr float ButtonSpacing = 10.f;
    const FMargin RowPad(0, 0, 0, 15.f);

    WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
    UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = RootCanvas;
    
    // Background dimmer
    UBorder* Dimmer = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Dimmer"));
    Dimmer->SetBrushColor(FLinearColor(0, 0, 0, 0.7f));
    UCanvasPanelSlot* DimmerSlot = RootCanvas->AddChildToCanvas(Dimmer);
    DimmerSlot->SetAnchors(FAnchors(0, 0, 1, 1));
    DimmerSlot->SetOffsets(FMargin(0));
    
    // Main dialog box
    USizeBox* DialogBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DialogBox"));
    DialogBox->SetWidthOverride(CardWidth);
    DialogBox->SetMaxDesiredHeight(CardMaxH);
    UCanvasPanelSlot* DialogSlot = RootCanvas->AddChildToCanvas(DialogBox);
    DialogSlot->SetAnchors(FAnchors(0.5f, 0.5f));
    DialogSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    DialogSlot->SetPosition(FVector2D(0, 0));
    DialogSlot->SetAutoSize(true);
    
    // Outline border
    OutlineBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Outline"));
    OutlineBorder->SetBrushColor(FLinearColor::FromSRGBColor(FColor(88, 91, 211)));
    OutlineBorder->SetPadding(FMargin(4.f));
    DialogBox->AddChild(OutlineBorder);

    // Card background
    Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Card"));
    Card->SetBrushColor(FLinearColor::FromSRGBColor(FColor(15, 31, 56)));
    Card->SetPadding(FMargin(Pad));
    OutlineBorder->SetContent(Card);
    
    // Main vertical container
    UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
    Card->SetContent(VBox);
    
    // Message text
    UTextBlock* MessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Message"));
    MessageText->SetText(Message);
    MessageText->SetAutoWrapText(true);
    MessageText->SetJustification(ETextJustify::Center);
    MessageText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 18));
    MessageText->SetColorAndOpacity(FLinearColor::White);
    UVerticalBoxSlot* MsgSlot = VBox->AddChildToVerticalBox(MessageText);
    MsgSlot->SetPadding(RowPad);
    MsgSlot->SetHorizontalAlignment(HAlign_Fill);
    
    // PIN display area
    PinDisplay = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PinDisplay"));
    PinDisplay->SetText(FText::FromString(TEXT("")));
    PinDisplay->SetJustification(ETextJustify::Center);
    PinDisplay->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 24));
    PinDisplay->SetColorAndOpacity(FLinearColor::White);
    
    UBorder* PinDisplayBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PinDisplayBorder"));
    PinDisplayBorder->SetBrushColor(FLinearColor::FromSRGBColor(FColor(25, 41, 66)));
    PinDisplayBorder->SetPadding(FMargin(15.f, 10.f));
    PinDisplayBorder->SetContent(PinDisplay);
    
    UVerticalBoxSlot* PinDisplaySlot = VBox->AddChildToVerticalBox(PinDisplayBorder);
    PinDisplaySlot->SetPadding(RowPad);
    PinDisplaySlot->SetHorizontalAlignment(HAlign_Fill);
    
    // Create button grid
    UGridPanel* ButtonGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), TEXT("ButtonGrid"));
    UVerticalBoxSlot* GridSlot = VBox->AddChildToVerticalBox(ButtonGrid);
    GridSlot->SetPadding(RowPad);
    GridSlot->SetHorizontalAlignment(HAlign_Center);
    
    // Create digit buttons in 3x4 grid layout
    // Row 0: 1, 2, 3
    // Row 1: 4, 5, 6  
    // Row 2: 7, 8, 9
    // Row 3: *, 0, #  (we'll use Delete, 0, Submit)
    
    DigitButtons.Reserve(10);
    
    // Create buttons 1-9 first
    for (int32 i = 1; i <= 9; ++i)
    {
        UButton* Button = CreateDigitButton(FString::FromInt(i));
        DigitButtons.Add(Button);
        
        int32 Row = (i - 1) / 3;
        int32 Col = (i - 1) % 3;
        
        UGridSlot* ButtonSlot = ButtonGrid->AddChildToGrid(Button, Row, Col);
        ButtonSlot->SetPadding(FMargin(ButtonSpacing / 2));
        ButtonSlot->SetHorizontalAlignment(HAlign_Fill);
        ButtonSlot->SetVerticalAlignment(VAlign_Fill);
    }
    
    // Bottom row: Delete, 0, Submit
    DeleteButton = CreateActionButton(TEXT("⌫"), FLinearColor::FromSRGBColor(FColor(200, 100, 100)));
    UGridSlot* DeleteSlot = ButtonGrid->AddChildToGrid(DeleteButton, 3, 0);
    DeleteSlot->SetPadding(FMargin(ButtonSpacing / 2));
    DeleteSlot->SetHorizontalAlignment(HAlign_Fill);
    DeleteSlot->SetVerticalAlignment(VAlign_Fill);
    
    UButton* ZeroButton = CreateDigitButton(TEXT("0"));
    DigitButtons.Add(ZeroButton); // Add 0 button to array
    UGridSlot* ZeroSlot = ButtonGrid->AddChildToGrid(ZeroButton, 3, 1);
    ZeroSlot->SetPadding(FMargin(ButtonSpacing / 2));
    ZeroSlot->SetHorizontalAlignment(HAlign_Fill);
    ZeroSlot->SetVerticalAlignment(VAlign_Fill);
    
    SubmitButton = CreateActionButton(TEXT("✓"), FLinearColor::FromSRGBColor(FColor(100, 200, 100)));
    UGridSlot* SubmitSlot = ButtonGrid->AddChildToGrid(SubmitButton, 3, 2);
    SubmitSlot->SetPadding(FMargin(ButtonSpacing / 2));
    SubmitSlot->SetHorizontalAlignment(HAlign_Fill);
    SubmitSlot->SetVerticalAlignment(VAlign_Fill);
}

UButton* UPinPadWidget::CreateDigitButton(const FString& Digit)
{
    UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), 
        FName(*(TEXT("DigitButton_") + Digit)));
    
    // Style the button
    FButtonStyle BtnStyle;
    BtnStyle.Normal.SetResourceObject(nullptr);
    BtnStyle.Normal.DrawAs = ESlateBrushDrawType::RoundedBox;
    BtnStyle.Normal.TintColor = FLinearColor::FromSRGBColor(FColor(70, 80, 120));
    BtnStyle.Normal.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
    BtnStyle.Normal.OutlineSettings.Radius = 8.0f;
    
    BtnStyle.Hovered = BtnStyle.Normal;
    BtnStyle.Hovered.TintColor = FLinearColor::FromSRGBColor(FColor(90, 100, 140));
    
    BtnStyle.Pressed = BtnStyle.Normal;
    BtnStyle.Pressed.TintColor = FLinearColor::FromSRGBColor(FColor(50, 60, 100));
    
    BtnStyle.NormalPadding = FMargin(15, 12);
    BtnStyle.PressedPadding = FMargin(15, 12);
    
    Button->SetStyle(BtnStyle);
    
    // Add text label
    UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    ButtonText->SetText(FText::FromString(Digit));
    ButtonText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 20));
    ButtonText->SetColorAndOpacity(FLinearColor::White);
    ButtonText->SetJustification(ETextJustify::Center);
    Button->AddChild(ButtonText);
    
    return Button;
}

UButton* UPinPadWidget::CreateActionButton(const FString& Label, const FLinearColor& Color)
{
    UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), 
        FName(*(TEXT("ActionButton_") + Label)));
    
    // Style the button
    FButtonStyle BtnStyle;
    BtnStyle.Normal.SetResourceObject(nullptr);
    BtnStyle.Normal.DrawAs = ESlateBrushDrawType::RoundedBox;
    BtnStyle.Normal.TintColor = Color * 0.7f;
    BtnStyle.Normal.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
    BtnStyle.Normal.OutlineSettings.Radius = 8.0f;
    
    BtnStyle.Hovered = BtnStyle.Normal;
    BtnStyle.Hovered.TintColor = Color * 0.9f;
    
    BtnStyle.Pressed = BtnStyle.Normal;
    BtnStyle.Pressed.TintColor = Color * 0.5f;
    
    BtnStyle.NormalPadding = FMargin(15, 12);
    BtnStyle.PressedPadding = FMargin(15, 12);
    
    Button->SetStyle(BtnStyle);
    
    // Add text label
    UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    ButtonText->SetText(FText::FromString(Label));
    ButtonText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
    ButtonText->SetColorAndOpacity(FLinearColor::White);
    ButtonText->SetJustification(ETextJustify::Center);
    Button->AddChild(ButtonText);
    
    return Button;
}

void UPinPadWidget::HandleDeleteClicked()
{
    RemoveLastDigit();
}

void UPinPadWidget::HandleSubmitClicked()
{
    if (CurrentPin.Len() > 0)
    {
        ClosePinPad();
    }
}

void UPinPadWidget::UpdatePinDisplay()
{
    if (PinDisplay)
    {
        PinDisplay->SetText(FText::FromString(GetMaskedPin()));
    }
}

void UPinPadWidget::AddDigit(const FString& Digit)
{
    if (CurrentPin.Len() < MaxPinLength)
    {
        CurrentPin += Digit;
        UpdatePinDisplay();
    }
}

void UPinPadWidget::RemoveLastDigit()
{
    if (CurrentPin.Len() > 0)
    {
        CurrentPin = CurrentPin.LeftChop(1);
        UpdatePinDisplay();
    }
}

void UPinPadWidget::CaptureFocusSoon()
{
    if (const UWorld* World = GetWorld())
    {
        FTimerHandle Handle;
        World->GetTimerManager().SetTimer(Handle, [WeakThis = TWeakObjectPtr(this)]
        {
            if (!WeakThis.IsValid()) return;
            
            // Set focus to the widget itself for keyboard input
            if (WeakThis->OwningPC.IsValid())
            {
                FInputModeUIOnly Mode;
                if (const TSharedPtr<SWidget> Focus = WeakThis->GetCachedWidget())
                {
                    Mode.SetWidgetToFocus(Focus);
                }
                Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                WeakThis->OwningPC->SetInputMode(Mode);
            }
        }, 0.1f, false);
    }
}

void UPinPadWidget::ApplyUIInputMode()
{
    if (const UWorld* World = GetWorld())
    {
        OwningPC = World->GetFirstPlayerController();
        if (OwningPC.IsValid())
        {
            // Store previous input settings
            bPrevMouseVisible = OwningPC->bShowMouseCursor;
            bPrevEnableClickEvents = OwningPC->bEnableClickEvents;
            bPrevEnableMouseOverEvents = OwningPC->bEnableMouseOverEvents;
            
            FInputModeUIOnly Mode;
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            OwningPC->SetInputMode(Mode);
            OwningPC->bShowMouseCursor = true;
            OwningPC->bEnableClickEvents = true;
            OwningPC->bEnableMouseOverEvents = true;
        }
    }
}

void UPinPadWidget::RestorePreviousInputMode() const
{
    if (OwningPC.IsValid())
    {
        FInputModeGameAndUI Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        OwningPC->SetInputMode(Mode);

        OwningPC->bShowMouseCursor = bPrevMouseVisible;
        OwningPC->bEnableClickEvents = bPrevEnableClickEvents;
        OwningPC->bEnableMouseOverEvents = bPrevEnableMouseOverEvents;
    }
}
