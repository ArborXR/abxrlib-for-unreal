#pragma once
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

	UFUNCTION()
	static AActor* SpawnInFrontOfPlayer(UWorld* World, const FString& Type);

private:
	TWeakObjectPtr<AActor> ActivePopupActor;
	TWeakObjectPtr<class UAbxrWidget> ActivePopupWidget;
	
	UFUNCTION()
	void HandlePopupClicked(const FText& InputText);

	UFUNCTION()
	void HandleInputRequested(const FAbxrAuthMechanism& Request);
};
