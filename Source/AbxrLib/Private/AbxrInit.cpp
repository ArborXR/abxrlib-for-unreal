#include "AbxrInit.h"
#include "Authentication.h"

void UAbxrInit::Init()
{
	Super::Init();
	
	Authentication::Authenticate();
}
