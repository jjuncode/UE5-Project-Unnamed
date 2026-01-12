// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/PlayerAnimInstance.h"
#include "PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Player
	OwnerCharacter = Cast<APlayerBase>(TryGetPawnOwner());

	// CharacterMovement
	if (OwnerCharacter)
		MovementComponent = OwnerCharacter->GetCharacterMovement();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 캐싱 재시도 
	if (OwnerCharacter == nullptr)
	{
		OwnerCharacter = Cast<APlayerBase>(TryGetPawnOwner());

		// CharacterMovement
		if (OwnerCharacter)
		{
			MovementComponent = OwnerCharacter->GetCharacterMovement();
		}
		else
			return;
	}

	// Set Velocity & Ground Speed
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	// Setting bShouldMove
	FVector Accel = MovementComponent->GetCurrentAcceleration();
	if (GroundSpeed >= 0.01 && Accel.Length() != 0)
	{
		bShouldMove = true;
	}
	else
	{
		bShouldMove = false;
	}

	// Setting IsFalling
	bIsFalling = MovementComponent->IsFalling();

	// Calculate direction using the delta between the velocity and the actor rotation.
	// When the character is not strafing, clamp the value between - and + 45 degrees 
	// so that backwards animations do not play when turning around, 
	// but running into wall looks better.
	
	float ret = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

	if (MovementComponent->bOrientRotationToMovement)
	{
		Direction = FMath::Clamp(ret, -45.f, 45.f);
	}
	else
	{
		Direction = ret;
	}

	Protocol::ActionState ActionState = OwnerCharacter->GetObjectInfo().creature_info().action_state();

	switch (ActionState)
	{
	case Protocol::ACTION_STATE_NONE:
		State = StateTags::State_Action_None;
		break;
	case Protocol::ACTION_STATE_SKILL:
		State = StateTags::State_Action_Skill;
		break;
	case Protocol::ACTION_STATE_DAMAGED:
		State = StateTags::State_Action_OnDamaged;
		break;
	default:
		break;
	}

	// Skill Animation
	if (ActionState == Protocol::ACTION_STATE_SKILL)
	{
		if (CurPlayingSkill != OwnerCharacter->GetObjectInfo().creature_info().skill_info() )// 다른 스킬이면 갱신 
		{
			// 재생
			CurPlayingSkill = OwnerCharacter->GetObjectInfo().creature_info().skill_info();
			switch (CurPlayingSkill)
			{
			case Protocol::SKILL_INFO_PUNCH:
				OwnerCharacter->PlayAnimMontage(PunchAttackMontage, 1.0, "PUNCH");
				break;
			case Protocol::SKILL_INFO_UPPERCUT:
				OwnerCharacter->PlayAnimMontage(UppercutAttackMontage, 1.0, "UPPERCUT");
				break;
			case Protocol::SKILL_INFO_KICK:
				OwnerCharacter->PlayAnimMontage(KickAttackMontage, 1.0, "KICK");
				break;

			default:
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Incorrect Skill Info"));
				break;
			}
		}

		if (IsAnyMontagePlaying() == false )
		{
			// 재생 종료
			OwnerCharacter->SetActionState(Protocol::ACTION_STATE_NONE);

			// 스킬정보 밀어버림
			OwnerCharacter->SetObjectInfoRef().mutable_creature_info()->set_skill_info(Protocol::SKILL_INFO_NONE);

			CurPlayingSkill = Protocol::SKILL_INFO_NONE;
		}
	}

}

void UPlayerAnimInstance::ClearAttackState(UAnimMontage* Montage, bool bInterrupted)
{
}
