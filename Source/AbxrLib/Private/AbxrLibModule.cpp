#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"

class FAbxrLibModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("AbxrLib: StartupModule"));
    }
    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("AbxrLib: ShutdownModule"));
    }
};

IMPLEMENT_MODULE(FAbxrLibModule, AbxrLib)
