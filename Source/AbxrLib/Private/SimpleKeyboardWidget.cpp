#include "SimpleKeyboardWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USimpleKeyboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentText.Empty();

	if (Btn_A)
		Btn_A->OnClicked.AddDynamic(this, &USimpleKeyboardWidget::OnA);

	if (Btn_B)
		Btn_B->OnClicked.AddDynamic(this, &USimpleKeyboardWidget::OnB);

	if (Btn_Backspace)
		Btn_Backspace->OnClicked.AddDynamic(this, &USimpleKeyboardWidget::OnBackspace);

	if (Btn_Done)
		Btn_Done->OnClicked.AddDynamic(this, &USimpleKeyboardWidget::OnDone);

	RefreshDisplay();
}

void USimpleKeyboardWidget::OnA()
{
	CurrentText.AppendChar(TEXT('A'));
	RefreshDisplay();
}

void USimpleKeyboardWidget::OnB()
{
	CurrentText.AppendChar(TEXT('B'));
	RefreshDisplay();
}

void USimpleKeyboardWidget::OnBackspace()
{
	if (!CurrentText.IsEmpty())
	{
		CurrentText.LeftChopInline(1);
		RefreshDisplay();
	}
}

void USimpleKeyboardWidget::OnDone()
{
	if (OnKeyboardDone.IsBound())
	{
		OnKeyboardDone.Execute(CurrentText);
	}

	// Optional: close the widget
	RemoveFromParent();
}

void USimpleKeyboardWidget::RefreshDisplay()
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(CurrentText));
	}
}
