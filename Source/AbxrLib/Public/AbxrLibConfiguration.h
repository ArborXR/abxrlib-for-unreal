#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AbxrLibConfiguration.generated.h"

class AKeyboardPawn;
class UInputWidget;

UCLASS(config=Game, defaultconfig, BlueprintType, meta=(DisplayName="AbxrLib Configuration"))
class ABXRLIB_API UAbxrLibConfiguration : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAbxrLibConfiguration()
	{
		RestUrl = TEXT("https://lib-backend.xrdm.app/");
		FrameRateTrackingPeriodSeconds = 0.5f;
		TelemetryTrackingPeriodSeconds = 10;
		SendRetriesOnFailure = 3;
		SendRetryIntervalSeconds = 3;
		SendNextBatchWaitSeconds = 30;
		StragglerTimeoutSeconds = 15;
		EventsPerSendAttempt = 16;
		LogsPerSendAttempt = 16;
		TelemetryEntriesPerSendAttempt = 16;
		StorageEntriesPerSendAttempt = 16;
		PruneSentItemsOlderThanHours = 12;
		MaximumCachedItems = 1024;
		RetainLocalAfterSent = false;
		DisableAutomaticTelemetry = false;
		DisableSceneEvents = false;
	}

	// Where it appears in Project Settings
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("AbxrLib"); }
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Application ID (required)"))
	FString AppId;
	void SetAppId(const FString& NewAppId) {this->AppId = NewAppId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Organization ID (optional)"))
	FString OrgId;
	void SetOrgId(const FString& NewOrgId) {this->OrgId = NewOrgId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Authorization Secret (optional)"))
	FString AuthSecret;
	void SetAuthSecret(const FString& NewAuthSecret) {this->AuthSecret = NewAuthSecret;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network", meta=(DisplayName="REST URL"))
	FString RestUrl;
	void SetRestUrl(const FString& NewRestUrl) {this->RestUrl = NewRestUrl;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Tracking Period (seconds)"))
	double TelemetryTrackingPeriodSeconds;
	void SetTelemetryTrackingPeriodSeconds(const double NewTelemetryTrackingPeriodSeconds) {this->TelemetryTrackingPeriodSeconds = NewTelemetryTrackingPeriodSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Frame Rate Tracking Period (seconds)"))
	double FrameRateTrackingPeriodSeconds;
	void SetFrameRateTrackingPeriodSeconds(const double NewFrameRateTrackingPeriodSeconds) {this->FrameRateTrackingPeriodSeconds = NewFrameRateTrackingPeriodSeconds;}
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retries on Failure"))
	int SendRetriesOnFailure;
	void SetSendRetriesOnFailure(const int NewSendRetriesOnFailure) {this->SendRetriesOnFailure = NewSendRetriesOnFailure;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retry Interval Seconds"))
	int SendRetryIntervalSeconds;
	void SetSendRetryIntervalSeconds(const int NewSendRetryIntervalSeconds) {this->SendRetryIntervalSeconds = NewSendRetryIntervalSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Next Batch Wait Seconds"))
	int SendNextBatchWaitSeconds;
	void SetSendNextBatchWaitSeconds(const int NewSendNextBatchWaitSeconds) {this->SendNextBatchWaitSeconds = NewSendNextBatchWaitSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Straggler Timeout Seconds"))
	int StragglerTimeoutSeconds;
	void SetStragglerTimeoutSeconds(const int NewStragglerTimeoutSeconds) {this->StragglerTimeoutSeconds = NewStragglerTimeoutSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Events Per Send Attempt"))
	int EventsPerSendAttempt;
	void SetEventsPerSendAttempt(const int NewEventsPerSendAttempt) {this->EventsPerSendAttempt = NewEventsPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Logs Per Send Attempt"))
	int LogsPerSendAttempt;
	void SetLogsPerSendAttempt(const int NewLogsPerSendAttempt) {this->LogsPerSendAttempt = NewLogsPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Entries Per Send Attempt"))
	int TelemetryEntriesPerSendAttempt;
	void SetTelemetryEntriesPerSendAttempt(const int NewTelemetryEntriesPerSendAttempt) {this->TelemetryEntriesPerSendAttempt = NewTelemetryEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Storage Entries Per Send Attempt"))
	int StorageEntriesPerSendAttempt;
	void SetStorageEntriesPerSendAttempt(const int NewStorageEntriesPerSendAttempt) {this->StorageEntriesPerSendAttempt = NewStorageEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Prune Sent Items Older Than Hours"))
	int PruneSentItemsOlderThanHours;
	void SetPruneSentItemsOlderThanHours(const int NewPruneSentItemsOlderThanHours) {this->PruneSentItemsOlderThanHours = NewPruneSentItemsOlderThanHours;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Maximum Cached Items"))
	int MaximumCachedItems;
	void SetMaximumCachedItems(const int NewMaximumCachedItems) {this->MaximumCachedItems = NewMaximumCachedItems;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Retain Local After Sent"))
	bool RetainLocalAfterSent;
	void SetRetainLocalAfterSent(const bool NewRetainLocalAfterSent) {this->RetainLocalAfterSent = NewRetainLocalAfterSent;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Automatic Telemetry"))
	bool DisableAutomaticTelemetry;
	void SetDisableAutomaticTelemetry(const bool NewDisableAutomaticTelemetry) {this->DisableAutomaticTelemetry = NewDisableAutomaticTelemetry;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Scene Events"))
	bool DisableSceneEvents;
	void SetDisableSceneEvents(const bool NewDisableSceneEvents) {this->DisableSceneEvents = NewDisableSceneEvents;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Input Menu Widget Class"))
	TSubclassOf<UInputWidget> InputMenuWidgetClass;
	void SetInputMenuWidgetClass(const TSubclassOf<UInputWidget>& NewInputMenuWidgetClass) {this->InputMenuWidgetClass = NewInputMenuWidgetClass;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="VR Keyboard Actor Class"))
	TSubclassOf<AKeyboardPawn> KeyboardActorClass;
	void SetKeyboardActorClass(const TSubclassOf<AKeyboardPawn>& NewKeyboardActorClass) {this->KeyboardActorClass = NewKeyboardActorClass;}

};
