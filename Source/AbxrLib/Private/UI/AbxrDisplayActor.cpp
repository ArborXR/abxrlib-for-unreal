#include "AbxrDisplayActor.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Services/Config/AbxrSettings.h"

AAbxrDisplayActor::AAbxrDisplayActor()
{
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AbxrWidgetComponent"));
	RootComponent = WidgetComponent;

	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(700, 700));
	WidgetComponent->SetWorldScale3D(FVector(0.2f));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AAbxrDisplayActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector NewLocation = CamLoc + CamRot.Vector() * 140.0f;
	const FRotator NewRotation = FRotator(0, CamRot.Yaw + 180.f, 0);
	SetActorLocationAndRotation(NewLocation, NewRotation);
}

void AAbxrDisplayActor::BeginPlay()
{
	Super::BeginPlay();

	const UAbxrSettings* Settings = GetDefault<UAbxrSettings>();
	
	if (PopupType == EAbxrPopupType::PinPad)
	{
		WidgetClass = Settings->CustomPinPadWidgetClass.IsNull() ?
			LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_PinPad.WBP_PinPad_C")) :
			Settings->CustomPinPadWidgetClass.LoadSynchronous();
	}
	else if (PopupType == EAbxrPopupType::Keyboard)
	{
		WidgetClass = Settings->CustomKeyboardWidgetClass.IsNull() ?
			LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_Keyboard.WBP_Keyboard_C")) :
			Settings->CustomKeyboardWidgetClass.LoadSynchronous();
	}
	else if (PopupType == EAbxrPopupType::QrScan)
	{
		WidgetClass = Settings->CustomQrScannerWidgetClass.IsNull() ?
			LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_QRScanner.WBP_QRScanner_C")) :
			Settings->CustomQrScannerWidgetClass.LoadSynchronous();
	}
	else if (PopupType == EAbxrPopupType::PollMultipleChoice)
	{
		WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_PollMulti.WBP_PollMulti_C"));
	}
	else if (PopupType == EAbxrPopupType::PollRating)
	{
		WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_PollRating.WBP_PollRating_C"));
	}
	else
	{
		WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_Keyboard.WBP_Keyboard_C"));
	}

	if (!WidgetClass) return;
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	if (!Widget) return;

	WidgetComponent->SetWidget(Widget);
}
