#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SimpleKeyboardWidget.h"
#include "KeyboardManager.generated.h"

UCLASS()
class ABXRLIB_API UKeyboardManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USimpleKeyboardWidget> KeyboardWidgetClass;

	static UKeyboardManager* Get();

	USimpleKeyboardWidget* CreateKeyboardWidget(UWorld* World);

private:
	static UKeyboardManager* Instance;
};