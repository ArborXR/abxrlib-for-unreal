// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputWidget.h"
#include "GameFramework/Actor.h"
#include "KeyboardPawn.generated.h"

UCLASS()
class ABXRLIB_API AKeyboardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKeyboardPawn();

	FOnInputCompleted_Native OnInputCompletedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Keyboard")
	FOnKeyboardTypeSetSignature OnSetKeyboardTypeDelegate;
	void Init(FString KeyboardType, FString KeyboardMessage);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FString KeyboardType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FString KeyboardMessage;

};
