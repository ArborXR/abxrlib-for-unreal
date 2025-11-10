#include "KeyboardManager.h"

UKeyboardManager* UKeyboardManager::Instance = nullptr;

UKeyboardManager* UKeyboardManager::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UKeyboardManager>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}

USimpleKeyboardWidget* UKeyboardManager::CreateKeyboardWidget(UWorld* World)
{
	if (!World || !KeyboardWidgetClass)
	{
		return nullptr;
	}

	USimpleKeyboardWidget* Widget = CreateWidget<USimpleKeyboardWidget>(World, KeyboardWidgetClass);
	if (Widget)
	{
		Widget->AddToViewport();
	}
	return Widget;
}