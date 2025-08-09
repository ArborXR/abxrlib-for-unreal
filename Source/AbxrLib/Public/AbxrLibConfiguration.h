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

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Organization ID (optional)"))
	FString OrgId;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Auth", meta=(DisplayName="Authorization Secret (optional)"))
	FString AuthSecret;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network", meta=(DisplayName="REST URL"))
	FString RestUrl;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Tracking Period (seconds)"))
	double TelemetryTrackingPeriodSeconds;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Frame Rate Tracking Period (seconds)"))
	double FrameRateTrackingPeriodSeconds;
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retries on Failure"))
	int SendRetriesOnFailure;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Retry Interval Seconds"))
	int SendRetryIntervalSeconds;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Send Next Batch Wait Seconds"))
	int SendNextBatchWaitSeconds;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Straggler Timeout Seconds"))
	int StragglerTimeoutSeconds;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Events Per Send Attempt"))
	int EventsPerSendAttempt;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Logs Per Send Attempt"))
	int LogsPerSendAttempt;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Telemetry Entries Per Send Attempt"))
	int TelemetryEntriesPerSendAttempt;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Storage Entries Per Send Attempt"))
	int StorageEntriesPerSendAttempt;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Prune Sent Items Older Than Hours"))
	int PruneSentItemsOlderThanHours;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Maximum Cached Items"))
	int MaximumCachedItems;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Retain Local After Sent"))
	bool RetainLocalAfterSent;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Automatic Telemetry"))
	bool DisableAutomaticTelemetry;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Data Sending Rules", meta=(DisplayName="Disable Scene Events"))
	bool DisableSceneEvents;
};
