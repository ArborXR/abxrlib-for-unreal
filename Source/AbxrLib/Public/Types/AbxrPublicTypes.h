#pragma once
#include "AbxrPublicTypes.generated.h"

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

USTRUCT(BlueprintType)
struct FAbxrModuleData
{
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Name;
	UPROPERTY() FString Target;
	UPROPERTY() int Order = 0;
};

USTRUCT()
struct FAbxrAuthMechanism
{
	GENERATED_BODY()

	UPROPERTY() FString Type;
	UPROPERTY() FString Prompt;
	UPROPERTY() FString Domain;
	
	FAbxrAuthMechanism() : Type(TEXT("")), Prompt(TEXT("")), Domain(TEXT("")) { }
	
	FAbxrAuthMechanism(const FString& InType, const FString& InPrompt, const FString& InDomain)
		: Type(InType), Prompt(InPrompt), Domain(InDomain) { }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrInputRequested, const FAbxrAuthMechanism&, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrAuthCompleted, const bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrModuleTarget, const FString&, ModuleTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbxrAllModulesCompleted);
