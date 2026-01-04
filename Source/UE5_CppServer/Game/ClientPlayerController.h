// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Protocol.pb.h"
#include "ClientPlayerController.generated.h"

struct FInputActionValue;
class AClientPlayer;

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
	void _HandleMoveAction(const FInputActionValue& Value, const Protocol::MoveState& State);
	void HandleMoveActionTrigerred(const FInputActionValue& Value);
	void HandleMoveActionCompleted(const FInputActionValue& Value);

	void HandleMouseLookAction(const FInputActionValue& Value);

private:
	UPROPERTY(Category=Input, EditAnywhere)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputAction> MouseLookAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputMappingContext> IMC;

	UPROPERTY(Category = Character, VisibleAnywhere)
	TObjectPtr<class AClientPlayer> ClientPlayer;
};
