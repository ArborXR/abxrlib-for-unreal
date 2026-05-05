#include "AbxrWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void UAbxrWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshInputText();
    
	if (UWidget* QRButton = WidgetTree->FindWidget(TEXT("QRScan")))
	{
		//QRButton->SetVisibility(FAbxrQRService::IsSupported()
		//	? ESlateVisibility::Visible
		//	: ESlateVisibility::Collapsed);
		QRButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UAbxrWidget::AppendString(const FString& Input)
{
	CurrentInput += Input;
	RefreshInputText();
}

void UAbxrWidget::Backspace()
{
	if (!CurrentInput.IsEmpty())
	{
		CurrentInput.LeftChopInline(1);
		RefreshInputText();
	}
}

void UAbxrWidget::RefreshInputText() const
{
	if (InputTextBlock) InputTextBlock->SetText(FText::FromString(CurrentInput));
}
