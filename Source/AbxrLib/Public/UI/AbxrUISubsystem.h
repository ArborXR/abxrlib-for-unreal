#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbxrUISubsystem.generated.h"

UCLASS()
class ABXRLIB_API UAbxrUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void ShowKeyboardUI(const FText& PromptText = FText::FromString(TEXT("Enter PIN")));

	UFUNCTION()
	void HideKeyboardUI();

	UFUNCTION()
	static AActor* SpawnPopupInFrontOfPlayer(UWorld* World);

private:
	TWeakObjectPtr<AActor> ActivePopupActor;
	TWeakObjectPtr<class UVRPopupWidget> ActivePopupWidget;
	
	UFUNCTION()
	void HandlePopupClicked(const FText& InputText);
};
