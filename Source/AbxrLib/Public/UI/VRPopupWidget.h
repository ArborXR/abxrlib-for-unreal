#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRPopupWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPopupButtonClicked, const FText&, ButtonText);

UCLASS()
class ABXRLIB_API UVRPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Bind in the widget BP to the Button's OnClicked
	UFUNCTION(BlueprintCallable)
	void NotifyClicked();

	// C++ can bind to this
	UPROPERTY(BlueprintAssignable, Category="Popup")
	FOnPopupButtonClicked OnPopupButtonClicked;

	// Assign this in BP (or bind via UMG binding)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Popup")
	FText CurrentButtonText;
};
