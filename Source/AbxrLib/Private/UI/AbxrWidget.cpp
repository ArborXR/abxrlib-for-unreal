#include "AbxrWidget.h"
#include "Blueprint/WidgetTree.h"

void UAbxrWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UWidget* QRButton = WidgetTree->FindWidget(TEXT("QRScan")))
	{
		// check if it should show
		//QRButton->SetVisibility(FAbxrQRService::IsSupported()
		//	? ESlateVisibility::Visible
		//	: ESlateVisibility::Collapsed);
	}
}

void UAbxrWidget::SetQRPreviewTexture(UTexture2D* Texture)
{
	if (QRPreviewImage && Texture)
	{
		QRPreviewImage->SetBrushFromTexture(Texture, true);
	}
}
