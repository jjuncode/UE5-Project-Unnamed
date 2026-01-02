// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClientPlayerController.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API AClientPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void HandleMoveAction(const FInputActionValue& Value);
	void HandleMouseLookAction(const FInputActionValue& Value);

private:
	UPROPERTY(Category=Input, EditAnywhere)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputAction> MouseLookAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputMappingContext> IMC;

};
