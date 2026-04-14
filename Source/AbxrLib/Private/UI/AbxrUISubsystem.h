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
	
	bool IsPopupVisible() const { return bIsPopupVisible; }
	
	FAbxrPopupShown OnPopupShown;
	FAbxrPopupHidden OnPopupHidden;
	
	UFUNCTION()
	bool ShowUI();

	UFUNCTION()
	void HideUI();

private:
	TWeakObjectPtr<AActor> ActivePopupActor;
	TWeakObjectPtr<class UAbxrWidget> ActivePopupWidget;
	FAbxrInputRequest ActiveInputRequest;
	FAbxrInputRequest SuspendedInputRequest;
	bool bHasSuspendedInputRequest = false;
	bool bIsPopupVisible = false;
	
	UPROPERTY()
	TArray<FAbxrInputRequest> PendingInputRequests;
	
	UFUNCTION()
	void HandleSubmitClicked(const FText& InputText);
	
	UFUNCTION()
	void HandleScanQRClicked();

	UFUNCTION()
	void HandleQrScanCancelled();
	
	void HandleInputRequested(const FAbxrInputRequest& Request);
	void HandleQrScanFinished(const FString& RawPayload, bool bCancelled);
	void RestoreSuspendedInputRequest();
	void DestroyActivePopup(bool bEndInteraction);
	bool ShowPopupForRequest(const FAbxrInputRequest& Request);
	
	void TryProcessNextInputRequest();
	
	AActor* SpawnActor(const EAbxrPopupType& PopupType) const;
};
