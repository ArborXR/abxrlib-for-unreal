#pragma once
#include "Subsystems/AbxrSubsystem.h"

UAbxrSubsystem* AbxrLib_GetActiveSubsystem();
void AbxrLib_SetActiveSubsystem(UAbxrSubsystem* Subsystem);
void AbxrLib_ClearActiveSubsystem(const UAbxrSubsystem* Subsystem);
