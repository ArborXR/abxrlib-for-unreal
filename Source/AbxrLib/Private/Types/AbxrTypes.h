#pragma once
#include "GameFramework/SaveGame.h"
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
struct FAbxrModuleData
{
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Name;
	UPROPERTY() FString Target;
	UPROPERTY() int Order = 0;
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
struct FAbxrAuthMechanism
{
	GENERATED_BODY()

	UPROPERTY() FString Type;
	UPROPERTY() FString Prompt;
	UPROPERTY() FString Domain;
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

UENUM(BlueprintType)
enum class EEventStatus : uint8
{
	Pass        UMETA(DisplayName = "pass"),
	Fail        UMETA(DisplayName = "fail"),
	Complete    UMETA(DisplayName = "complete"),
	Incomplete  UMETA(DisplayName = "incomplete"),
	Browsed     UMETA(DisplayName = "browsed")
};

UENUM(BlueprintType)
enum class ELogLevel : uint8
{
	Debug     UMETA(DisplayName = "debug"),
	Info      UMETA(DisplayName = "info"),
	Warn      UMETA(DisplayName = "warn"),
	Error     UMETA(DisplayName = "error"),
	Critical  UMETA(DisplayName = "critical")
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Bool         UMETA(DisplayName = "bool"),
	Select       UMETA(DisplayName = "select"),
	Text         UMETA(DisplayName = "text"),
	Rating       UMETA(DisplayName = "rating"),
	Number       UMETA(DisplayName = "number"),
	Matching     UMETA(DisplayName = "matching"),
	Performance  UMETA(DisplayName = "performance"),
	Sequencing   UMETA(DisplayName = "sequencing")
};

UCLASS()
class USuperMetaSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY() TMap<FString, FString> SuperMetaData;
};
