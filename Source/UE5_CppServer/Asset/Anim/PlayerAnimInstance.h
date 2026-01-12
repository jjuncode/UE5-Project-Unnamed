// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "protocol.pb.h"
#include "PlayerAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None,
	Skill,
	Damaged
};

UCLASS()
class UE5_CPPSERVER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	void ClearAttackState(UAnimMontage* Montage, bool bInterrupted);
	
protected:
	// ------------------
	//		Montage
	// ------------------
	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> PunchAttackMontage;

	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> UppercutAttackMontage;

	UPROPERTY(Category = Animation, EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> KickAttackMontage;
	
	Protocol::SkillInfo CurPlayingSkill = Protocol::SKILL_INFO_NONE;

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
	EActionState State;
};