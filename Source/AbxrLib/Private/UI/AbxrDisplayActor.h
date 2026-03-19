#pragma once
#include "Components/WidgetComponent.h"
#include "Types/AbxrTypes.h"
#include "AbxrDisplayActor.generated.h"

UCLASS()
class AAbxrDisplayActor : public AActor
{
	GENERATED_BODY()
public:
	AAbxrDisplayActor();

	virtual void BeginPlay() override;
	
	UPROPERTY()
	EAbxrPopupType PopupType;
	
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;
	
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* WidgetComponent;
	
	virtual void Tick(float DeltaTime) override;
};