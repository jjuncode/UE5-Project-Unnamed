// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "protocol.pb.h"
#include "Tags.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class UE5_CPPSERVER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// ------------------
	//		Caching
	// ------------------
	Protocol::SkillInfo PlayingSkillInfo = Protocol::SKILL_INFO_NONE;

	// ------------------
	//		Montage
	// ------------------
	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> AttackMontage;
	
	// -------------------------------
	//		AnimGraph Caching
	// -------------------------------
	UPROPERTY(Category = Character,VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class APlayerBase> OwnerCharacter;

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

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag State;
};