#include "AbxrLib/Public/AbxrModule.h"
#include "AbxrLib/Public/Authentication.h"

void AbxrModule::AbxrInit()
{
	UE_LOG(LogTemp, Warning, TEXT("Running AbxrLib init logic..."))
	Authentication::Authenticate();
}

IMPLEMENT_MODULE(AbxrModule, AbxrLib)
