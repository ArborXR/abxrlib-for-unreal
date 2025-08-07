#pragma once

#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

class AbxrModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("AbxrLib plugin starting up"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("AbxrLib plugin shutting down"));
    }
};

IMPLEMENT_MODULE(AbxrModule, AbxrLib)
