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
	{
		Protocol::AttackDir DamageDir = OwnerCharacter->GetDamagedDir();

		switch (DamageDir)
		{
		case Protocol::DIR_NONE:
			break;
		case Protocol::DIR_UP_TO_DOWN:
			State = StateTags::State_Action_OnDamaged_DOWN;	// 아래로 피격당함
			break;
		case Protocol::DIR_DOWN_TO_UP:
			State = StateTags::State_Action_OnDamaged_UP;	// 위로 피격당함 
			break;
		case Protocol::DIR_RIGHT_TO_LEFT:
			State = StateTags::State_Action_OnDamaged_RIGHT;// 공격방향이 좌측이니 
			break;
		case Protocol::DIR_LEFT_TO_RIGHT:
			State = StateTags::State_Action_OnDamaged_LEFT;	// 공격방향이 좌측이니 
			break;
		case Protocol::DIR_FRONT:
			State = StateTags::State_Action_OnDamaged_FRONT;
			break;
		default:
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Invalid Damaged Dir"));
			break;
		}


		break;
	}
	default:
		break;
	}

	// Skill Animation
	if (ActionState == Protocol::ACTION_STATE_SKILL)
	{
		Protocol::SkillInfo CurSkill = OwnerCharacter->GetCurPlayingSkill();
		
		if (CurSkill != PlayingSkillInfo) // 한 스킬만 시전 가능 
		{
			// 재생
			switch (CurSkill)
			{
			case Protocol::SKILL_INFO_SLASH_UP:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0, "SLASH_UP");
				break;
			case Protocol::SKILL_INFO_SLASH_DOWN:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0, "SLASH_DOWN");
				break;
			case Protocol::SKILL_INFO_SLASH_LEFT:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0, "SLASH_LEFT");
				break;
			case Protocol::SKILL_INFO_SLASH_RIGHT:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0, "SLASH_RIGHT");
				break;
			case Protocol::SKILL_INFO_KICK:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0, "KICK");
				break;

			default:
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Incorrect Skill Info"));
				break;
			}

			PlayingSkillInfo = CurSkill;
		}

		if (IsAnyMontagePlaying() == false )
		{
			// 재생 종료
			OwnerCharacter->SetActionState(Protocol::ACTION_STATE_NONE);

			// 스킬정보 밀어버림
			OwnerCharacter->SetCurPlayingSkill(Protocol::SKILL_INFO_NONE);
			PlayingSkillInfo = Protocol::SKILL_INFO_NONE;
		}
	}

}