#pragma once
#include "Components/WidgetComponent.h"
#include "AbxrDisplayActor.generated.h"

UCLASS()
class AAbxrDisplayActor : public AActor
{
	GENERATED_BODY()
public:
	AAbxrDisplayActor();

	virtual void BeginPlay() override;
	
	UPROPERTY()
	FString PopupType;
	
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;
	
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* WidgetComponent;
	
	virtual void Tick(float DeltaTime) override;
};