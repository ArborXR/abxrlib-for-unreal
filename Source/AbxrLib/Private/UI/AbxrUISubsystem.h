#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/AbxrTypes.h"
#include "AbxrUISubsystem.generated.h"

class UTexture2D;
class FAbxrQRService;

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
	bool bIsPopupVisible = false;
	TUniquePtr<FAbxrQRService> QRService;

	UPROPERTY()
	TObjectPtr<UTexture2D> QRPreviewTexture = nullptr;
	
	UPROPERTY()
	TArray<FAbxrInputRequest> PendingInputRequests;
	
	UFUNCTION()
	void HandleSubmitClicked(const FText& InputText);
	
	UFUNCTION()
	void HandleScanQRClicked();

	UFUNCTION()
	void HandleQRCancelled();

	UFUNCTION()
	void HandleCameraPermissionResult(const TArray<FString>& Permissions, const TArray<bool>& GrantResults);

	void HandleQRFailed(const FString& Error);
	void HandleQRPreviewFrame(const TArray<uint8>& Pixels, int32 Width, int32 Height);
	
	void HandleInputRequested(const FAbxrInputRequest& Request);
	
	void TryProcessNextInputRequest();
	
	AActor* SpawnActor(const EAbxrPopupType& PopupType) const;
	void StartQRScannerUI();
};
