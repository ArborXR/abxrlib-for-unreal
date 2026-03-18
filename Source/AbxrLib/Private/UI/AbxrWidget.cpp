#include "AbxrWidget.h"
#include "Blueprint/WidgetTree.h"

void UAbxrWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	if (UWidget* QRButton = WidgetTree->FindWidget(TEXT("QRScan")))
	{
		//QRButton->SetVisibility(FAbxrQRService::IsSupported()
		//	? ESlateVisibility::Visible
		//	: ESlateVisibility::Collapsed);
		QRButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}
