#pragma once
#include "CoreMinimal.h"
#include "AbxrTypes.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrInputRequested, const FAbxrAuthMechanism&, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrAuthCompleted, const bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbxrModuleTarget, const FString&, ModuleTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbxrAllModulesCompleted);