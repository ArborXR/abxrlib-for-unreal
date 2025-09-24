// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputCompleted_Native, FString&/*, Text*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyboardTypeSetSignature);
/**
 * 
 */
UCLASS()
class ABXRLIB_API UInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UInputWidget* CreateInputWidget(UWorld* CurrentWorld, const FString KeyboardType, const TSubclassOf<UInputWidget> MenuWidgetClass);

	UFUNCTION(BlueprintCallable)
	void CallInputCompleted(FString InputText);
	
	FOnInputCompleted_Native OnInputCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnKeyboardTypeSetSignature OnKeyboardTypeSetDelegate;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FString KeyboardType;

public:
	UFUNCTION(BlueprintCallable)
	void SetKeyboardType(FString NewKeyboardType) { KeyboardType = NewKeyboardType; };
};
