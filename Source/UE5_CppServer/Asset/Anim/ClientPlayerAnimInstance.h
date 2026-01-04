// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ClientPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API UClientPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(Category = Character,VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AClientPlayer> ClientPlayer;

	UPROPERTY(Category = Character,VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	float Direction;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	bool bShouldMove;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	bool bIsFalling;
};
