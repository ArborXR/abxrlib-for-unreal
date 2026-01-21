#include "AbxrSettings.h"
#include "Utils.h"

UAbxrSettings::UAbxrSettings()
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
	MaxCallFrequencySeconds = 1;
	DataEntriesPerSendAttempt = 32;
	StorageEntriesPerSendAttempt = 16;
	PruneSentItemsOlderThanHours = 12;
	MaximumCachedItems = 1024;
	RetainLocalAfterSent = false;
}

bool UAbxrSettings::IsValid() const
{
    // appID must pass format validation if set (UUID format)
    if (!Utils::IsUuidFormat(AppId))
    {
		UE_LOG(LogTemp, Error, TEXT("AbxrLib: Invalid Application ID format. Must be a valid UUID. Cannot authenticate."));
        return false;
    }

    // orgID is optional but must pass format validation if set (UUID format)
    if (!OrgId.IsEmpty() && !Utils::IsUuidFormat(OrgId))
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Invalid Organization ID format. Must be a valid UUID. Cannot authenticate."));
        return false;
    }
    
    // Validate restUrl format - must be a valid HTTP/HTTPS URL
    if (RestUrl.IsEmpty())
    {
    	UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - RestUrl is required but not set"));
        return false;
    }
    
    if (!Utils::IsValidUrl(RestUrl))
    {
    	UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - RestUrl '%s' is not a valid HTTP/HTTPS URL"), *RestUrl);
        return false;
    }
    
    // Validate numeric ranges for timeouts and intervals
    if (SendRetriesOnFailure < 0 || SendRetriesOnFailure > 10)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "SendRetriesOnFailure must be between 0 and 10, got %s"),
                                    *FString::FromInt(SendRetriesOnFailure));
        return false;
    }

    if (SendRetryIntervalSeconds < 1 || SendRetryIntervalSeconds > 300)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "SendRetryIntervalSeconds must be between 1 and 300, got %s"),
                                    *FString::FromInt(SendRetryIntervalSeconds));
        return false;
    }

    if (SendNextBatchWaitSeconds < 1 || SendNextBatchWaitSeconds > 3600)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "SendNextBatchWaitSeconds must be between 1 and 3600, got %s"),
                                    *FString::FromInt(SendNextBatchWaitSeconds));
        return false;
    }

    /*if (RequestTimeoutSeconds < 5 || RequestTimeoutSeconds > 300)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "RequestTimeoutSeconds must be between 5 and 300, got %s"),
                                    *FString::FromInt(RequestTimeoutSeconds));
        return false;
    }*/

    if (StragglerTimeoutSeconds < 0 || StragglerTimeoutSeconds > 3600)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "StragglerTimeoutSeconds must be between 0 and 3600, got %s"),
                                    *FString::FromInt(StragglerTimeoutSeconds));
        return false;
    }

    if (MaxCallFrequencySeconds < 0.1f || MaxCallFrequencySeconds > 60.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "MaxCallFrequencySeconds must be between 0.1 and 60, got %s"),
                                    *LexToString(MaxCallFrequencySeconds));
        return false;
    }

    if (DataEntriesPerSendAttempt < 1 || DataEntriesPerSendAttempt > 1000)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "DataEntriesPerSendAttempt must be between 1 and 1000, got %s"),
                                    *FString::FromInt(DataEntriesPerSendAttempt));
        return false;
    }

    if (StorageEntriesPerSendAttempt < 1 || StorageEntriesPerSendAttempt > 1000)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "StorageEntriesPerSendAttempt must be between 1 and 1000, got %s"),
                                    *FString::FromInt(StorageEntriesPerSendAttempt));
        return false;
    }

    if (PruneSentItemsOlderThanHours < 0 || PruneSentItemsOlderThanHours > 8760) // Max 1 year
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "PruneSentItemsOlderThanHours must be between 0 and 8760, got %s"),
                                    *FString::FromInt(PruneSentItemsOlderThanHours));
        return false;
    }
    
    if (MaximumCachedItems < 10 || MaximumCachedItems > 10000)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "MaximumCachedItems must be between 10 and 10000, got %s"),
                                    *FString::FromInt(MaximumCachedItems));
        return false;
    }

    /*if (MaxDictionarySize < 5 || MaxDictionarySize > 1000)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "MaxDictionarySize must be between 5 and 1000, got %s"),
                                    *FString::FromInt(MaxDictionarySize));
        return false;
    }*/

    if (PositionCapturePeriodSeconds < 0.1f || PositionCapturePeriodSeconds > 60.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "PositionCapturePeriodSeconds must be between 0.1 and 60, got %s"),
                                    *LexToString(PositionCapturePeriodSeconds));
        return false;
    }

    if (FrameRateTrackingPeriodSeconds < 0.1f || FrameRateTrackingPeriodSeconds > 60.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "FrameRateTrackingPeriodSeconds must be between 0.1 and 60, got %s"),
                                    *LexToString(FrameRateTrackingPeriodSeconds));
        return false;
    }

    if (TelemetryTrackingPeriodSeconds < 1.0f || TelemetryTrackingPeriodSeconds > 300.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
                                    "MaximumCachedItems must be between 0.1 and 300, got %s"),
                                    *LexToString(TelemetryTrackingPeriodSeconds));
        return false;
    }

    if (AuthenticationStartDelay < 0 || AuthenticationStartDelay > 60.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("AbxrLib: Configuration validation failed - "
        							"AuthenticationStartDelay must be between 0 and 60, got %s"),
        							*LexToString(AuthenticationStartDelay));
        return false;
    }
	
    return true;
}
