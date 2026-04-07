#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "AbxrLibAPI_Internal.h"
#include "Types/AbxrLog.h"

class FAbxrLibModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogAbxrLib, Log, TEXT("StartupModule"));
    }
    virtual void ShutdownModule() override
    {
        UE_LOG(LogAbxrLib, Log, TEXT("ShutdownModule"));
    }
};

IMPLEMENT_MODULE(FAbxrLibModule, AbxrLib)

// Cached “active” subsystem (weak so it auto-invalidates)
static TWeakObjectPtr<UAbxrSubsystem> GAbxrSubsystem;

UAbxrSubsystem* AbxrLib_GetActiveSubsystem()
{
    return GAbxrSubsystem.Get();
}

void AbxrLib_SetActiveSubsystem(UAbxrSubsystem* Subsystem)
{
    GAbxrSubsystem = Subsystem;
}

void AbxrLib_ClearActiveSubsystem(const UAbxrSubsystem* Subsystem)
{
    // Only clear if it matches (important when multiple PIE instances exist)
    if (GAbxrSubsystem.Get() == Subsystem)
    {
        GAbxrSubsystem = nullptr;
    }
}