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
		EnableAutomaticTelemetry = true;
		HeadsetControllerTracking = true;
		PositionCapturePeriodSeconds = 1;
		EnableSceneEvents = true;
		EnableAutoStartAuth = true;
		AuthenticationStartDelay = 0;
		FrameRateTrackingPeriodSeconds = 0.5f;
		TelemetryTrackingPeriodSeconds = 10;
		SendRetriesOnFailure = 3;
		SendRetryIntervalSeconds = 3;
		SendNextBatchWaitSeconds = 30;
		StragglerTimeoutSeconds = 15;
		DataEntriesPerSendAttempt = 32;
		StorageEntriesPerSendAttempt = 16;
		PruneSentItemsOlderThanHours = 12;
		MaximumCachedItems = 1024;
		RetainLocalAfterSent = false;
	}

	// Where it appears in Project Settings
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("AbxrLib"); }
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Authentication", meta=(DisplayName="Application ID (required)"))
	FString AppId;
	void SetAppId(const FString& NewAppId) {this->AppId = NewAppId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Authentication", meta=(DisplayName="Organization ID (optional)"))
	FString OrgId;
	void SetOrgId(const FString& NewOrgId) {this->OrgId = NewOrgId;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Authentication", meta=(DisplayName="Authorization Secret (optional)"))
	FString AuthSecret;
	void SetAuthSecret(const FString& NewAuthSecret) {this->AuthSecret = NewAuthSecret;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Service Provider", meta=(DisplayName="REST URL"))
	FString RestUrl;
	void SetRestUrl(const FString& NewRestUrl) {this->RestUrl = NewRestUrl;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Player Tracking", meta=(DisplayName="Enable Automatic Telemetry"))
	bool EnableAutomaticTelemetry;
	void SetEnableAutomaticTelemetry(const bool NewEnableAutomaticTelemetry) {this->EnableAutomaticTelemetry = NewEnableAutomaticTelemetry;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Player Tracking", meta=(DisplayName="Headset/Controller Tracking"))
	bool HeadsetControllerTracking;
	void SetHeadsetControllerTracking(const bool NewHeadsetControllerTracking) {this->HeadsetControllerTracking = NewHeadsetControllerTracking;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Player Tracking", meta=(DisplayName="Position Capture Period (seconds)"))
	int PositionCapturePeriodSeconds;
	void SetPositionCapturePeriodSeconds(const int NewPositionCapturePeriodSeconds) {this->PositionCapturePeriodSeconds = NewPositionCapturePeriodSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Player Tracking", meta=(DisplayName="Enable Scene Events"))
	bool EnableSceneEvents;
	void SetEnableSceneEvents(const bool NewEnableSceneEvents) {this->EnableSceneEvents = NewEnableSceneEvents;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Authentication Control", meta=(DisplayName="Enable Auto Start Authentication"))
	bool EnableAutoStartAuth;
	void SetEnableAutoStartAuth(const bool NewEnableAutoStartAuth) {this->EnableAutoStartAuth = NewEnableAutoStartAuth;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Authentication Control", meta=(DisplayName="Authentication Start Delay (seconds)"))
	int AuthenticationStartDelay;
	void SetAuthenticationStartDelay(const bool NewAuthenticationStartDelay) {this->AuthenticationStartDelay = NewAuthenticationStartDelay;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Telemetry Tracking Period (seconds)"))
	double TelemetryTrackingPeriodSeconds;
	void SetTelemetryTrackingPeriodSeconds(const double NewTelemetryTrackingPeriodSeconds) {this->TelemetryTrackingPeriodSeconds = NewTelemetryTrackingPeriodSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Frame Rate Tracking Period (seconds)"))
	double FrameRateTrackingPeriodSeconds;
	void SetFrameRateTrackingPeriodSeconds(const double NewFrameRateTrackingPeriodSeconds) {this->FrameRateTrackingPeriodSeconds = NewFrameRateTrackingPeriodSeconds;}
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Send Retries on Failure"))
	int SendRetriesOnFailure;
	void SetSendRetriesOnFailure(const int NewSendRetriesOnFailure) {this->SendRetriesOnFailure = NewSendRetriesOnFailure;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Send Retry Interval Seconds"))
	int SendRetryIntervalSeconds;
	void SetSendRetryIntervalSeconds(const int NewSendRetryIntervalSeconds) {this->SendRetryIntervalSeconds = NewSendRetryIntervalSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Send Next Batch Wait Seconds"))
	int SendNextBatchWaitSeconds;
	void SetSendNextBatchWaitSeconds(const int NewSendNextBatchWaitSeconds) {this->SendNextBatchWaitSeconds = NewSendNextBatchWaitSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Straggler Timeout Seconds"))
	int StragglerTimeoutSeconds;
	void SetStragglerTimeoutSeconds(const int NewStragglerTimeoutSeconds) {this->StragglerTimeoutSeconds = NewStragglerTimeoutSeconds;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Data Entries Per Send Attempt"))
	int DataEntriesPerSendAttempt;
	void SetDataEntriesPerSendAttempt(const int NewDataEntriesPerSendAttempt) {this->DataEntriesPerSendAttempt = NewDataEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Storage Entries Per Send Attempt"))
	int StorageEntriesPerSendAttempt;
	void SetStorageEntriesPerSendAttempt(const int NewStorageEntriesPerSendAttempt) {this->StorageEntriesPerSendAttempt = NewStorageEntriesPerSendAttempt;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Prune Sent Items Older Than Hours"))
	int PruneSentItemsOlderThanHours;
	void SetPruneSentItemsOlderThanHours(const int NewPruneSentItemsOlderThanHours) {this->PruneSentItemsOlderThanHours = NewPruneSentItemsOlderThanHours;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Maximum Cached Items"))
	int MaximumCachedItems;
	void SetMaximumCachedItems(const int NewMaximumCachedItems) {this->MaximumCachedItems = NewMaximumCachedItems;}

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Network Configuration", meta=(DisplayName="Retain Local After Sent"))
	bool RetainLocalAfterSent;
	void SetRetainLocalAfterSent(const bool NewRetainLocalAfterSent) {this->RetainLocalAfterSent = NewRetainLocalAfterSent;}
};
