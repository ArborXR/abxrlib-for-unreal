// VRPopupWidget.cpp
#include "VRPopupWidget.h"

void UVRPopupWidget::NotifyClicked()
{
	OnPopupButtonClicked.Broadcast(CurrentButtonText);
}
