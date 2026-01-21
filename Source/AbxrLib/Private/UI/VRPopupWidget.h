#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRPopupWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPopupButtonClicked, const FText&, InputText);

UCLASS()
class ABXRLIB_API UVRPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void NotifyClicked() const { OnPopupButtonClicked.Broadcast(InputText); }
	
	FOnPopupButtonClicked OnPopupButtonClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Popup")
	FText InputText;
};
