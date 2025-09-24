// Fill out your copyright notice in the Description page of Project Settings.


#include "Keyboard/InputWidget.h"


UInputWidget* UInputWidget::CreateInputWidget(UWorld* CurrentWorld, const FString KeyboardType, const TSubclassOf<UInputWidget> MenuWidgetClass)
{
	UInputWidget* NewWidget = Cast<UInputWidget>(CreateWidget<UInputWidget>(CurrentWorld, MenuWidgetClass));
	NewWidget->SetKeyboardType(KeyboardType);
	NewWidget->OnKeyboardTypeSetDelegate.Broadcast();
	
	NewWidget->AddToViewport(0);
	
	return NewWidget;
}

void UInputWidget::CallInputCompleted(FString InputText)
{
	OnInputCompleted.Broadcast(InputText);
}
