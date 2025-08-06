#pragma once

#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogAbxrLib, Log, All);

class AbxrModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("AbxrLib plugin starting up!"));
        AbxrInit();
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("AbxrLib plugin shutting down"));
    }

private:
    void AbxrInit()
    {
        UE_LOG(LogAbxrLib, Warning, TEXT("Running AbxrLib init logic..."));
    }
};

IMPLEMENT_MODULE(AbxrModule, AbxrLib)
