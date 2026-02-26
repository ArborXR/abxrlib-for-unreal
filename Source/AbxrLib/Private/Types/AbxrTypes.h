#pragma once
#include "GameFramework/SaveGame.h"
#include "Types/AbxrPublicTypes.h"
#include "AbxrTypes.generated.h"

class UWidgetInteractionComponent;
class AAbxrLaserPointerActor;

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
	UPROPERTY() FString OsVersion;
	UPROPERTY() FString XrdmVersion;
	UPROPERTY() FString AppVersion;
	UPROPERTY() FString UnrealVersion;
	UPROPERTY() FString AbxrLibVersion;
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

	UPROPERTY() FAbxrAuthMechanism AuthMechanism;
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
	TFunction<void(const FAbxrAuthMechanism&)> OnInputRequested;
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

UENUM(BlueprintType)
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

// Persisted, per-user progress through LMS-provided modules.
// Mirrors Unity StorageScope.user key "AbxrModuleIndex".
UCLASS()
class UAbxrModuleProgressSave : public USaveGame
{
	GENERATED_BODY()

public:
	// Index into the auth-provided module list (sorted by Order).
	UPROPERTY() int32 ModuleIndex = 0;

	// Optional (debug / validation).
	UPROPERTY() FString UserId;
};
