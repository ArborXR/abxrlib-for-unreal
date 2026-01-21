#pragma once
#include "Subsystems/AbxrSubsystem.h"

ABXRLIB_API UAbxrSubsystem* AbxrLib_GetActiveSubsystem();
ABXRLIB_API void AbxrLib_SetActiveSubsystem(UAbxrSubsystem* Subsystem);
ABXRLIB_API void AbxrLib_ClearActiveSubsystem(const UAbxrSubsystem* Subsystem);
