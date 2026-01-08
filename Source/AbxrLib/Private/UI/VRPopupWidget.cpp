// VRPopupWidget.cpp
#include "UI/VRPopupWidget.h"

void UVRPopupWidget::NotifyClicked()
{
	OnPopupButtonClicked.Broadcast(CurrentButtonText);
}
