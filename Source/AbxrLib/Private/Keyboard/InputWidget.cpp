// Fill out your copyright notice in the Description page of Project Settings.


#include "Keyboard/InputWidget.h"


UInputWidget* UInputWidget::CreateInputWidget(UWorld* CurrentWorld, const FString KeyboardType, const FString& PromptText, const TSubclassOf<UInputWidget> MenuWidgetClass)
{
	UInputWidget* NewWidget = Cast<UInputWidget>(CreateWidget<UInputWidget>(CurrentWorld, MenuWidgetClass));
	NewWidget->AddToViewport(0);
	
	NewWidget->SetKeyboardType(KeyboardType);
	NewWidget->SetKeyboardMessage(PromptText);
	NewWidget->OnKeyboardTypeSetDelegate.Broadcast();
	
	return NewWidget;
}

void UInputWidget::CallInputCompleted(FString InputText)
{
	OnInputCompleted.Broadcast(InputText);
}
