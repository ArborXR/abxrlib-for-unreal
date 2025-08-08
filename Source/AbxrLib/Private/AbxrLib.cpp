#include "AbxrLib.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FAbxrLibModule, AbxrLib)

void FAbxrLibModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("AbxrLib: StartupModule"));
}

void FAbxrLibModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("AbxrLib: ShutdownModule"));
}
