#include "AbxrWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Services/Auth/AbxrQRService.h"

void UAbxrWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	if (UWidget* QRButton = WidgetTree->FindWidget(TEXT("QRButton")))
	{
		QRButton->SetVisibility(FAbxrQRService::IsSupported()
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed);
	}
}
