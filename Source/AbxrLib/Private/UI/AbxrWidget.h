#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbxrWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubmitButtonClicked, const FText&, InputText);

UCLASS()
class ABXRLIB_API UAbxrWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SubmitInput() const { OnSubmitButtonClicked.Broadcast(InputText); }
	
	FOnSubmitButtonClicked OnSubmitButtonClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InputText;
};
