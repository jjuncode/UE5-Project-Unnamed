// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/PlayerAnimInstance.h"
#include "PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

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
	
	 float ret = UKismetAnimationLibrary::CalculateDirection(Velocity,	OwnerCharacter->GetActorRotation());

	if (MovementComponent->bOrientRotationToMovement)
	{
		Direction = FMath::Clamp(ret, -45.f, 45.f);
	}
	else
	{
		Direction = ret;
	}

	Protocol::ActionState ActionState = OwnerCharacter->GetActionState();

	switch (ActionState)
	{
	case Protocol::ACTION_STATE_NONE:
		State = ActionState::State_Action_None;
		break;
	case Protocol::ACTION_STATE_ATTACK_TRY:
		PlayAttackAnimationTry();
		break;
	case Protocol::ACTION_STATE_ATTACK_SUCCESS:
		PlayAttackAnimationTry();	// 공격 애니메이션 그대로 수행 
		break;
	case Protocol::ACTION_STATE_ATTACK_INTERRUPTED:
		PlayAttackAnimationInterrupted();
		break;
	case Protocol::ACTION_STATE_DAMAGED:
	{
		PlayHittedAnimation();
		break;
	}
	case Protocol::ACTION_STATE_PARRY:
	{
		// 패링 성공 
		// 공격 애니메이션을 1.5배속으로 틀어준다.
		// 이후, 타격 지점에서 패링 애니메이션이 수행된다.
		PlayParryAnimation();
		break;
	}	

	default:
		break;
	}

	// 재생 종료
	if (IsAnyMontagePlaying() == false)
	{
		if (OwnerCharacter->GetActionState() == Protocol::ACTION_STATE_MOVE_RUN)
		{
			OwnerCharacter->SetActionState(Protocol::ACTION_STATE_MOVE_RUN);
			OwnerCharacter->ResetDamageDir();
			State = ActionState::State_Action_Move_Run;
		}
		else
		{
			OwnerCharacter->SetActionState(Protocol::ACTION_STATE_MOVE_IDLE);
			OwnerCharacter->ResetDamageDir();
			State = ActionState::State_Action_Move_Idle;
		}

		PlayParryTiming = false;

		// 스킬 시전중 데미지 입었을경우를 대비 
		// 스킬정보 밀어버림
		OwnerCharacter->SetCurPlayingSkill(Protocol::SKILL_INFO_NONE);
		CurPlayingAttackSkill = Protocol::SKILL_INFO_NONE;
	}
}

void UPlayerAnimInstance::PlayAttackAnimationTry()
{
	// 한번만 수행해야한다.
	if (State != ActionState::State_Action_Attack_Try)
	{
 		State = ActionState::State_Action_Attack_Try;

		// Skill Animation
		Protocol::SkillId CurSkill = OwnerCharacter->GetCurPlayingSkill();
		CurPlayingAttackSkill = CurSkill;

		// 재생
		switch (CurSkill)
		{
		case Protocol::SKILL_INFO_SLASH_UP:
			OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, "SLASH_UP");
			break;
		case Protocol::SKILL_INFO_SLASH_DOWN:
			OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, "SLASH_DOWN");
			break;
		case Protocol::SKILL_INFO_SLASH_LEFT:
			OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, "SLASH_LEFT");
			break;
		case Protocol::SKILL_INFO_SLASH_RIGHT:
			OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, "SLASH_RIGHT");
			break;
		case Protocol::SKILL_INFO_KICK:
			OwnerCharacter->PlayAnimMontage(AttackMontage, 1.0f, "KICK");
			break;

		default:
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Incorrect Skill Info"));
			break;
		}
	}
}

void UPlayerAnimInstance::PlayAttackAnimationSuccess()
{
}

void UPlayerAnimInstance::PlayAttackAnimationInterrupted()
{
	// 틀어주던거 계속 틀고 있음

	if (PlayAttackInterruptedTiming )
	{
		// 패링 애니메이션은 한번만 재생함
		State = ActionState::State_Action_Attack_Interrupted;
		PlayAttackInterruptedTiming = false;
		OwnerCharacter->PlayAnimMontage(AttackInterrupteddMontage, 1.0, "ATTACK_INTERRUPTED");
	}

}

void UPlayerAnimInstance::PlayHittedAnimation()
{
	// 한번만 수행해야한다.
	if (State != ActionState::State_Action_OnDamaged)
	{
		State = ActionState::State_Action_OnDamaged;

		Protocol::AttackDir DamageDir = OwnerCharacter->GetDamagedDir();

		switch (DamageDir)
		{
		case Protocol::DIR_NONE:
			break;
		case Protocol::DIR_UP_TO_DOWN:
			OwnerCharacter->PlayAnimMontage(HittedMontage, 1.0, "UP_TO_DOWN");
			break;
		case Protocol::DIR_DOWN_TO_UP:
			OwnerCharacter->PlayAnimMontage(HittedMontage, 1.0, "DOWN_TO_UP");
			break;
		case Protocol::DIR_RIGHT_TO_LEFT:
			OwnerCharacter->PlayAnimMontage(HittedMontage, 1.0, "RIGHT");
			break;
		case Protocol::DIR_LEFT_TO_RIGHT:
			OwnerCharacter->PlayAnimMontage(HittedMontage, 1.0, "LEFT");
			break;
		case Protocol::DIR_FRONT:
			OwnerCharacter->PlayAnimMontage(HittedMontage, 1.0, "FRONT");
			break;
		default:
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Invalid Damaged Dir"));
			break;
		}
	}
}

void UPlayerAnimInstance::PlayParryAnimation()
{
	// 한번만 수행해야한다.
	if (State != ActionState::State_Action_Parry)
	{
		State = ActionState::State_Action_Parry;

		// 초반 공격애니메이션 재생
		Protocol::SkillId CurSkill = OwnerCharacter->GetCurPlayingSkill();

		// 이미 재생중이었는지 봐야함 
		if (CurPlayingAttackSkill == Protocol::SKILL_INFO_NONE)
		{
			// 재생중인게 없을 때만 재생해준다.
			switch (CurSkill)
			{
			case Protocol::SKILL_INFO_SLASH_UP:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.25f, "SLASH_UP");
				break;
			case Protocol::SKILL_INFO_SLASH_DOWN:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.25f, "SLASH_DOWN");
				break;
			case Protocol::SKILL_INFO_SLASH_LEFT:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.25f, "SLASH_LEFT");
				break;
			case Protocol::SKILL_INFO_SLASH_RIGHT:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.25f, "SLASH_RIGHT");
				break;
			case Protocol::SKILL_INFO_KICK:
				OwnerCharacter->PlayAnimMontage(AttackMontage, 1.25f, "KICK");
				break;

			default:
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[ UPlayerAnimInstance ] : Error - Incorrect Skill Info"));
				break;
			}
		}
	}

	if (PlayParryTiming && State == ActionState::State_Action_Parry)
	{
		// 패링 애니메이션은 한번만 재생함
		PlayParryTiming = false;
		OwnerCharacter->PlayAnimMontage(ParryMontage, 1.0, "Parry");
	}
}
