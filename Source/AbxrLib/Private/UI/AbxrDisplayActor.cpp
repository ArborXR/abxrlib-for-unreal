#include "AbxrDisplayActor.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"

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
	
	if (PopupType == TEXT("assessmentPin"))
		WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_PinPad.WBP_PinPad_C"));
	else
		WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/AbxrLib/UI/WBP_Keyboard.WBP_Keyboard_C"));
	
	if (!WidgetClass) return;
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	if (!Widget) return;

	WidgetComponent->SetWidget(Widget);
}
