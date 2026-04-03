#pragma once
#include "Engine/EngineTypes.h"
#include "GameFramework/SaveGame.h"
#include "Types/AbxrPublicTypes.h"
#include "AbxrTypes.generated.h"

class UWidgetInteractionComponent;
class AAbxrLaserPointerActor;

UENUM()
enum class EAbxrPopupType : uint8
{
	Keyboard,
	PinPad,
	PollMultipleChoice,
	PollRating
};

USTRUCT()
struct FAbxrInputRequest
{
	GENERATED_BODY()

	UPROPERTY()
	EAbxrPopupType PopupType = EAbxrPopupType::Keyboard;
	
	UPROPERTY()
	FString Prompt;

	// For multiple choice poll
	UPROPERTY()
	TArray<FString> Responses;
};

USTRUCT()
struct FAbxrWidgetInteractionBackup
{
	GENERATED_BODY()

	bool bValid = false;

	TEnumAsByte<ECollisionChannel> TraceChannel;
	float InteractionDistance;
	bool bShowDebug;
};

USTRUCT()
struct FAbxrWidgetInteractionHandle
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UWidgetInteractionComponent> WidgetInteraction = nullptr;

	// If we found an existing one and changed settings, store backup
	FAbxrWidgetInteractionBackup Backup;

	// Laser actor we spawned (always ours; we destroy it on End)
	UPROPERTY()
	TObjectPtr<AAbxrLaserPointerActor> LaserActor = nullptr;
};

USTRUCT()
struct FAbxrAuthPayload
{
	GENERATED_BODY()

	UPROPERTY() FString AppId;
	UPROPERTY() FString OrgId;
	UPROPERTY() FString AuthSecret;
	UPROPERTY() FString DeviceId;
	UPROPERTY() FString UserId;
	UPROPERTY() TArray<FString> Tags;
	UPROPERTY() FString SessionId;
	UPROPERTY() FString Partner;
	UPROPERTY() FString IpAddress;
	UPROPERTY() FString DeviceModel;
	UPROPERTY() TMap<FString, FString> Geolocation;
	UPROPERTY() FString osversion;  // need this casing to get it to convert to json correctly
	UPROPERTY() FString XrdmVersion;
	UPROPERTY() FString AppVersion;
	UPROPERTY() FString UnrealVersion;
	UPROPERTY() FString AbxrLibVersion;
	UPROPERTY() FString AbxrLibType;
	UPROPERTY() TMap<FString, FString> AuthMechanism;
};

USTRUCT()
struct FAbxrAuthResponse
{
	GENERATED_BODY()

	UPROPERTY() FString Token;
	UPROPERTY() FString Secret;
	UPROPERTY() TMap<FString, FString> UserData;
	UPROPERTY() FString UserId;
	UPROPERTY() FString AppId;
	UPROPERTY() FString PackageName;
	UPROPERTY() TArray<FAbxrModuleData> Modules;
};

USTRUCT()
struct FAbxrConfigPayload
{
	GENERATED_BODY()

	UPROPERTY() TMap<FString, FString> AuthMechanism;
	UPROPERTY() FString FrameRateCapturePeriod;
	UPROPERTY() FString TelemetryCapturePeriod;
	UPROPERTY() FString RestUrl;
	UPROPERTY() FString SendRetriesOnFailure;
	UPROPERTY() FString SendRetryInterval;
	UPROPERTY() FString SendNextBatchWait;
	UPROPERTY() FString StragglerTimeout;
	UPROPERTY() FString DataEntriesPerSendAttempt;
	UPROPERTY() FString StorageEntriesPerSendAttempt;
	UPROPERTY() FString PruneSentItemsOlderThan;
	UPROPERTY() FString MaximumCachedItems;
	UPROPERTY() FString RetainLocalAfterSent;
	UPROPERTY() FString PositionCapturePeriod;
};

struct FAbxrAuthCallbacks
{
	TFunction<void(const FAbxrInputRequest&)> OnInputRequested;
	TFunction<void()> OnSucceeded;
	TFunction<void(const FString&)> OnFailed;
};

USTRUCT()
struct FAbxrEventPayload
{
	GENERATED_BODY()

	UPROPERTY() FString preciseTimestamp;
	UPROPERTY() FString name;
	UPROPERTY() TMap<FString, FString> meta;
};

USTRUCT()
struct FAbxrTelemetryPayload
{
	GENERATED_BODY()

	UPROPERTY() FString preciseTimestamp;
	UPROPERTY() FString name;
	UPROPERTY() TMap<FString, FString> meta;
};

USTRUCT()
struct FAbxrLogPayload
{
	GENERATED_BODY()

	UPROPERTY() FString preciseTimestamp;
	UPROPERTY() FString logLevel;
	UPROPERTY() FString text;
	UPROPERTY() TMap<FString, FString> meta;
};

USTRUCT()
struct FAbxrDataPayloadWrapper
{
	GENERATED_BODY()

	UPROPERTY() TArray<FAbxrEventPayload> event;
	UPROPERTY() TArray<FAbxrTelemetryPayload> telemetry;
	UPROPERTY() TArray<FAbxrLogPayload> basicLog;
};

UENUM()
enum class EPartner : uint8
{
	None,
	ArborXR
};

UCLASS()
class USuperMetaSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY() TMap<FString, FString> SuperMetaData;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FAbxrInputRequested, const FAbxrInputRequest& Request);
