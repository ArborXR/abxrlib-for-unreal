#pragma once
#include "AbxrPublicTypes.generated.h"

UENUM(BlueprintType)
enum class EPollType : uint8
{
	MultipleChoice  UMETA(DisplayName = "Multiple Choice"),
	Rating          UMETA(DisplayName = "Rating 1-5")
};

UENUM(BlueprintType)
enum class EEventStatus : uint8
{
	Pass,
	Fail,
	Complete,
	Incomplete,
	Browsed
};

UENUM(BlueprintType)
enum class ELogLevel : uint8
{
	Debug,
	Info,
	Warn,
	Error,
	Critical
};

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Bool,
	Select,
	Text,
	Rating,
	Number,
	Matching,
	Performance,
	Sequencing
};

USTRUCT(BlueprintType)
struct FAbxrModuleData
{
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Name;
	UPROPERTY() FString Target;
	UPROPERTY() int Order = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrAuthCompleted, const bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrModuleTarget, const FString&, ModuleTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbxrAllModulesCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbxrPopupShown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbxrPopupHidden);
