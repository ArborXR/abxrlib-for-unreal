#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AbxrLibConfiguration.generated.h"

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
	void SetAppId(const FString& AppId) {this->AppId = AppId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Organization ID (optional)"))
	FString OrgId;
	void SetOrgId(const FString& OrgId) {this->OrgId = OrgId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Authorization Secret (optional)"))
	FString AuthSecret;
	void SetAuthSecret(const FString& AuthSecret) {this->AuthSecret = AuthSecret;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network", meta=(DisplayName="REST URL"))
	FString RestUrl;
	void SetRestUrl(const FString& RestUrl) {this->RestUrl = RestUrl;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Tracking Period (seconds)"))
	double TelemetryTrackingPeriodSeconds;
	void SetTelemetryTrackingPeriodSeconds(const double TelemetryTrackingPeriodSeconds) {this->TelemetryTrackingPeriodSeconds = TelemetryTrackingPeriodSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Frame Rate Tracking Period (seconds)"))
	double FrameRateTrackingPeriodSeconds;
	void SetFrameRateTrackingPeriodSeconds(const double FrameRateTrackingPeriodSeconds) {this->FrameRateTrackingPeriodSeconds = FrameRateTrackingPeriodSeconds;}
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retries on Failure"))
	int SendRetriesOnFailure;
	void SetSendRetriesOnFailure(const int SendRetriesOnFailure) {this->SendRetriesOnFailure = SendRetriesOnFailure;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retry Interval Seconds"))
	int SendRetryIntervalSeconds;
	void SetSendRetryIntervalSeconds(const int SendRetryIntervalSeconds) {this->SendRetryIntervalSeconds = SendRetryIntervalSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Next Batch Wait Seconds"))
	int SendNextBatchWaitSeconds;
	void SetSendNextBatchWaitSeconds(const int SendNextBatchWaitSeconds) {this->SendNextBatchWaitSeconds = SendNextBatchWaitSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Straggler Timeout Seconds"))
	int StragglerTimeoutSeconds;
	void SetStragglerTimeoutSeconds(const int StragglerTimeoutSeconds) {this->StragglerTimeoutSeconds = StragglerTimeoutSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Events Per Send Attempt"))
	int EventsPerSendAttempt;
	void SetEventsPerSendAttempt(const int EventsPerSendAttempt) {this->EventsPerSendAttempt = EventsPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Logs Per Send Attempt"))
	int LogsPerSendAttempt;
	void SetLogsPerSendAttempt(const int LogsPerSendAttempt) {this->LogsPerSendAttempt = LogsPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Entries Per Send Attempt"))
	int TelemetryEntriesPerSendAttempt;
	void SetTelemetryEntriesPerSendAttempt(const int TelemetryEntriesPerSendAttempt) {this->TelemetryEntriesPerSendAttempt = TelemetryEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Storage Entries Per Send Attempt"))
	int StorageEntriesPerSendAttempt;
	void SetStorageEntriesPerSendAttempt(const int StorageEntriesPerSendAttempt) {this->StorageEntriesPerSendAttempt = StorageEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Prune Sent Items Older Than Hours"))
	int PruneSentItemsOlderThanHours;
	void SetPruneSentItemsOlderThanHours(const int PruneSentItemsOlderThanHours) {this->PruneSentItemsOlderThanHours = PruneSentItemsOlderThanHours;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Maximum Cached Items"))
	int MaximumCachedItems;
	void SetMaximumCachedItems(const int MaximumCachedItems) {this->MaximumCachedItems = MaximumCachedItems;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Retain Local After Sent"))
	bool RetainLocalAfterSent;
	void SetRetainLocalAfterSent(const bool RetainLocalAfterSent) {this->RetainLocalAfterSent = RetainLocalAfterSent;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Automatic Telemetry"))
	bool DisableAutomaticTelemetry;
	void SetDisableAutomaticTelemetry(const bool DisableAutomaticTelemetry) {this->DisableAutomaticTelemetry = DisableAutomaticTelemetry;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Scene Events"))
	bool DisableSceneEvents;
	void SetDisableSceneEvents(const bool DisableSceneEvents) {this->DisableSceneEvents = DisableSceneEvents;}
};
