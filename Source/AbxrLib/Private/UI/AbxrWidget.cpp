#include "UI/AbxrWidget.h"

#include "Blueprint/WidgetTree.h"
#include "QR/AbxrQrScannerCoordinator.h"
#include "Components/Widget.h"
#include "Engine/GameInstance.h"

void UAbxrWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	if (UWidget* QRButton = WidgetTree->FindWidget(TEXT("QRScan")))
	{
		bool bShowQrButton = false;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UAbxrQrScannerCoordinator* QrCoordinator = GI->GetSubsystem<UAbxrQrScannerCoordinator>())
			{
				bShowQrButton = QrCoordinator->IsQrScanningAvailable();
			}
		}

		QRButton->SetVisibility(bShowQrButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
