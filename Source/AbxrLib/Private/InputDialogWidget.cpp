#include "InputDialogWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/World.h"
#include "Styling/CoreStyle.h"
#include "TimerManager.h"
#include "Styling/SlateTypes.h"

UInputDialogWidget* UInputDialogWidget::ShowDialog(UWorld* World, const FText& Message, const FText& Placeholder,
                                                   const bool bStartWithExistingText, const FString& ExistingText, const int32 ZOrder)
{
    UInputDialogWidget* Dialog = CreateWidget<UInputDialogWidget>(World);
    if (!Dialog) return nullptr;

    Dialog->BuildWidgetTree(Message, Placeholder, bStartWithExistingText, ExistingText);
    Dialog->AddToViewport(ZOrder);
    Dialog->ApplyUIInputMode();
    Dialog->CaptureFocusSoon();

    return Dialog;
}

void UInputDialogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (OkButton)
    {
        OkButton->OnClicked.Clear();
        OkButton->OnClicked.AddDynamic(this, &UInputDialogWidget::HandleOkClicked);
    }
}

void UInputDialogWidget::HandleOkClicked()
{
    CloseDialog();
}

FReply UInputDialogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey Key = InKeyEvent.GetKey();
    if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
    {
        CloseDialog();
        return FReply::Handled();
    }
    
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInputDialogWidget::CloseDialog()
{
    const FString Text = EditBox ? EditBox->GetText().ToString() : FString();
    OnAccepted.Broadcast(Text);
    RemoveFromParent();
    RestorePreviousInputMode();
}

void UInputDialogWidget::SetText(const FString& NewText) const
{
    if (EditBox) EditBox->SetText(FText::FromString(NewText));
}

void UInputDialogWidget::BuildWidgetTree(const FText& Message, const FText& Placeholder, const bool bStartWithExistingText, const FString& ExistingText)
{
    constexpr float CardWidth = 560.f;
    constexpr float CardMaxH  = 360.f;
    constexpr float Pad       = 16.f;   // uniform padding inside the card
    constexpr float RowGap    = 12.f;   // vertical gap between rows
    const FMargin RowPad(0, 0, 0, RowGap);

    WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
    UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = RootCanvas;
    
    UBorder* Dimmer = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Dimmer"));
    Dimmer->SetBrushColor(FLinearColor(0, 0, 0, 0.55f));
    UCanvasPanelSlot* DimmerSlot = RootCanvas->AddChildToCanvas(Dimmer);
    DimmerSlot->SetAnchors(FAnchors(0, 0, 1, 1));
    DimmerSlot->SetOffsets(FMargin(0));
    
    USizeBox* DialogBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DialogBox"));
    DialogBox->SetWidthOverride(CardWidth);
    DialogBox->SetMaxDesiredHeight(CardMaxH);
    UCanvasPanelSlot* DialogSlot = RootCanvas->AddChildToCanvas(DialogBox);
    DialogSlot->SetAnchors(FAnchors(0.5f, 0.5f));     // center anchor
    DialogSlot->SetAlignment(FVector2D(0.5f, 0.5f));  // center align
    DialogSlot->SetPosition(FVector2D(0, 0));         // offset from center
    DialogSlot->SetAutoSize(true);
    
    OutlineBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Outline"));
    OutlineBorder->SetBrushColor(FLinearColor::FromSRGBColor(FColor(88, 91, 211)));
    OutlineBorder->SetPadding(FMargin(4.f));  // border thickness
    DialogBox->AddChild(OutlineBorder);

    Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Card"));
    Card->SetBrushColor(FLinearColor::FromSRGBColor(FColor(15, 31, 56)));
    Card->SetPadding(FMargin(Pad));
    OutlineBorder->SetContent(Card);
    DialogBox->AddChild(Card);

    UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
    Card->SetContent(VBox);
    
    UTextBlock* MessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Message"));
    MessageText->SetText(Message);
    MessageText->SetAutoWrapText(true);
    MessageText->SetJustification(ETextJustify::Center);
    MessageText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 18)); // no deprecation warning
    UVerticalBoxSlot* MsgSlot = VBox->AddChildToVerticalBox(MessageText);
    MsgSlot->SetPadding(RowPad);
    MsgSlot->SetHorizontalAlignment(HAlign_Fill); // take full width
    
    EditBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("EditBox"));
    EditBox->WidgetStyle.SetForegroundColor(FSlateColor(FLinearColor::White));
    EditBox->SetHintText(Placeholder);
    if (bStartWithExistingText) EditBox->SetText(FText::FromString(ExistingText));
    // Inner text inset (use style), outer spacing via slot:
    EditBox->WidgetStyle.Padding = FMargin(8.f);
    
    FEditableTextBoxStyle Style = EditBox->WidgetStyle;
    Style.ForegroundColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.3f);
    Style.FocusedForegroundColor = FLinearColor(0.07f, 0.07f, 0.07f);
    Style.SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 18));
    EditBox->WidgetStyle = Style;

    UVerticalBoxSlot* EditSlot = VBox->AddChildToVerticalBox(EditBox);
    EditSlot->SetPadding(RowPad);  // gap below input
    EditSlot->SetHorizontalAlignment(HAlign_Fill);
    
    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ButtonsRow"));
    UVerticalBoxSlot* BtnRowSlot = VBox->AddChildToVerticalBox(HBox);
    BtnRowSlot->SetHorizontalAlignment(HAlign_Center);
    BtnRowSlot->SetPadding(FMargin(0));  // final row; no extra bottom gap

    auto MakeButton = [&](const TCHAR* Name, const TCHAR* Label) -> UButton*
    {
        UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
        // Content padding inside the button
        FButtonStyle BtnStyle = Btn->GetStyle();  // copy
        BtnStyle.NormalPadding = FMargin(10, 6);
        BtnStyle.PressedPadding = FMargin(10, 6);
        Btn->SetStyle(BtnStyle);

        UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Txt->SetText(FText::FromString(Label));
        Txt->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 14));
        Txt->SetColorAndOpacity(FLinearColor(0.07f, 0.07f, 0.07f));
        Btn->AddChild(Txt);
        return Btn;
    };
    
    OkButton = MakeButton(TEXT("OkButton"), TEXT("SUBMIT"));

    UHorizontalBoxSlot* OkSlot = HBox->AddChildToHorizontalBox(OkButton);
    OkSlot->SetPadding(FMargin(0));
    OkSlot->SetHorizontalAlignment(HAlign_Center);
    OkSlot->SetVerticalAlignment(VAlign_Center);
    
    if (OkButton)
    {
        OkButton->OnClicked.Clear();
        OkButton->OnClicked.AddDynamic(this, &UInputDialogWidget::HandleOkClicked);
    }
    if (EditBox)
    {
        EditBox->OnTextCommitted.Clear();
        EditBox->OnTextCommitted.AddDynamic(this, &UInputDialogWidget::HandleTextCommitted);
    }
}

void UInputDialogWidget::HandleTextCommitted(const FText&, const ETextCommit::Type Method)
{
    if (Method == ETextCommit::OnEnter)
    {
        CloseDialog();
    }
}

void UInputDialogWidget::CaptureFocusSoon()
{
    if (const UWorld* World = GetWorld())
    {
        FTimerHandle Handle;
        World->GetTimerManager().SetTimer(Handle, [WeakThis = TWeakObjectPtr(this)]
        {
            if (!WeakThis.IsValid()) return;
            const auto* Self = WeakThis.Get();
            if (!Self->EditBox) return;

            // Give focus to the text box
            Self->EditBox->SetKeyboardFocus();

            // Tell InputMode which Slate widget should hold focus
            if (Self->OwningPC.IsValid())
            {
                FInputModeUIOnly Mode;
                if (const TSharedPtr<SWidget> Focus = Self->EditBox->GetCachedWidget())
                {
                    Mode.SetWidgetToFocus(Focus);
                }
                Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                Self->OwningPC->SetInputMode(Mode);
            }
        }, 0.0f, false);
    }
}

void UInputDialogWidget::ApplyUIInputMode()
{
    if (const UWorld* World = GetWorld())
    {
        OwningPC = World->GetFirstPlayerController();
        if (OwningPC.IsValid())
        {
            FInputModeUIOnly Mode;
            // Don't set focus here yet; do it after the Slate widget exists.
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            OwningPC->SetInputMode(Mode);
            OwningPC->bShowMouseCursor = true;
            OwningPC->bEnableClickEvents = true;
            OwningPC->bEnableMouseOverEvents = true;
        }
    }
}

void UInputDialogWidget::RestorePreviousInputMode() const
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
