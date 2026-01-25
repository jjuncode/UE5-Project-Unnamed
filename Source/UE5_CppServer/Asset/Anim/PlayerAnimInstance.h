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

private:
	void PlayAttackAnimationTry();
	void PlayAttackAnimationSuccess();
	void PlayAttackAnimationInterrupted();
	void PlayHittedAnimation();
	void PlayParryAnimation();

private:
	void CheckConvertStateToBattle(Protocol::ActionState CheckState);

protected:
	// Cache
	bool PlayParryTiming = false;
	bool PlayAttackInterruptedTiming = false;
	Protocol::SkillId CurPlayingAttackSkill;
	Protocol::ActionState PrevState;

protected:
	// ------------------
	//		Montage
	// ------------------
	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> AttackMontage;
	
	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> HittedMontage;

	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> ParryMontage;

	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> AttackInterrupteddMontage;

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

public:
	friend class UAnimNotify_SlowEffect_End;
};