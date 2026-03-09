#pragma once
#include "Services/Auth/AbxrQRService.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/AbxrTypes.h"
#include "AbxrUISubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION()
	void ShowKeyboardUI(const FText& Prompt, const FString& Type);

	UFUNCTION()
	void HideKeyboardUI();

private:
	TWeakObjectPtr<AActor> ActivePopupActor;
	TWeakObjectPtr<class UAbxrWidget> ActivePopupWidget;
	FAbxrQRService QRService;
	
	UFUNCTION()
	void HandleSubmitClicked(const FText& InputText);
	
	UFUNCTION()
	void HandleScanQRClicked();
	
	void HandleInputRequested(const FAbxrKeyboardRequest& Request);
	
	AActor* SpawnActor(const FString& Type) const;
};
