// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Protocol.pb.h"
#include "GameplayTagsClasses.h"
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

	// -----------------------
	//		Handle Event 
	// -----------------------
	void HandleEvent(FGameplayTag EventTag);

private:
	// -----------------------
	//		Handle IA 
	// -----------------------
	// IA_Move
	void _HandleMoveAction(const FInputActionValue& Value, const Protocol::ActionState& State);
	void HandleMoveActionTrigerred(const FInputActionValue& Value);
	void HandleMoveActionCompleted(const FInputActionValue& Value);
	void SyncYaw(const FInputActionValue& Value);

	// IA_MouseLook
	void HandleMouseLookAction(const FInputActionValue& Value);

	// IA_Attack
	void HandleSkillAction(const FInputActionValue& Value);

	// 적수지정
	void HandleSearchEnemyAction(const FInputActionValue& Value);

private:
	const float SearchEnemyDistn = 1000;	// 적수지정 거리
	const float SearchEnemyAngle = 45.f;	// 적수지정 각도

private:
	UPROPERTY(Category=Input, EditAnywhere)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputAction> MouseLookAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputAction> SkillAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputAction> SearchEnemyAction;

	UPROPERTY(Category = Input, EditAnywhere)
	TObjectPtr<class UInputMappingContext> IMC;

	UPROPERTY(Category = Character, VisibleAnywhere)
	TObjectPtr<class AClientPlayer> ClientPlayer;
};
