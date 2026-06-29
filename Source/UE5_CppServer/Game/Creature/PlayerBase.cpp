// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "ClientPlayer.h"
#include "BasePlayerController.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Weapon.h"

void APlayerBase::HandleEvent(FGameplayTag EventTag)
{
	Controller->HandleEvent(EventTag);
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveSync();
}

void APlayerBase::Caching(bool IsDummy)
{
	bIsMyPlayer = false;
	bIsDummy = IsDummy;

	// Set Controller
	Controller = Cast<ABasePlayerController>(GetController());
	check(Controller);
}

void APlayerBase::AttachWeapon(TSubclassOf<class AWeapon> WeaponInfo)
{
	if (WeaponInfo)
	{
		Weapon = WeaponInfo;

		// Spawn Weapon
		AWeapon* spawnWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon);

		// 소켓 이름을 통해 현재 메시에서 소켓을 참조
		const USkeletalMeshSocket* weaponSocket = GetMesh()->GetSocketByName("WeaponSocket");

		if (spawnWeapon && weaponSocket)
		{
			// 소켓에 액터를 할당
			spawnWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				TEXT("WeaponSocket")
			);
		}
	}
}

void APlayerBase::OnDamaged(const Protocol::S_DAMAGED& DamagePkt)
{
	SetObjectInfo(DamagePkt.object_info());

	// TODO : 피격된 거에 따른 hp 감소 등 처리
	Protocol::SkillData DamagingSkill = DamagePkt.skill_data();

	// 피격 방향 Cache
	DamagedDir = DamagingSkill.attack_dir();
}

void APlayerBase::Parry()
{
	SetObjectInfo(ParryAttackInfo.object_info());

	// 패링 스킬 정보 
	Protocol::SkillData DamagingSkill = ParryAttackInfo.skill_data();
	//DamagedDir = DamagingSkill.attack_dir();
}

void APlayerBase::CachingParryAttackInfo(Protocol::S_PARRY ParryInfo)
{
	ParryAttackInfo = ParryInfo;
}

bool APlayerBase::IsMoveState() const
{
	const Protocol::ActionState State = GetActionState();

	return State == Protocol::ACTION_STATE_MOVE ||
		State == Protocol::ACTION_STATE_BATTLE;
}

//구현 할 것들을 위해 함수 작성
 

bool APlayerBase::CanUseSkill() const
{
	return !IsActionBlockedState() && IsMoveState();
}

bool APlayerBase::CanDash() const
{
	return !IsActionBlockedState() && IsMoveState();
}

bool APlayerBase::CanDodge() const
{
	return !IsActionBlockedState() && IsMoveState();
}

bool APlayerBase::CanAttack() const
{
	return !IsActionBlockedState() && IsMoveState();
}

bool APlayerBase::CanParry() const
{
	return !IsActionBlockedState() && IsMoveState();
}

bool APlayerBase::CanTakeDamage() const
{
	return true;
}

// ----------------------------추후 return 값 수정 피료
bool APlayerBase::IsDeadState() const
{
	return false;
}

bool APlayerBase::IsGroggyState() const
{
	return false;
}
//------------------------

//한번에 관리
bool APlayerBase::IsActionBlockedState() const
{
	return IsDeadState() || IsGroggyState();
}
 
bool APlayerBase::CanRecoverFromGroggy() const
{
	return IsGroggyState();
}
bool APlayerBase::CanDie() const
{
	return true;
}
void APlayerBase::MoveSync()
{
	// ClientPlayer는 제외
	if (bIsMyPlayer)
		return;

	
		// BATTLE의 경우 임시 ( TODO : 칼뽑 이동 애니메이션 ) 
	if (IsMoveState())
	{
		// Rotate
		{
			SetActorRotation(FRotator(0, DestnInfo.yaw(), 0));
		}

		// Move
		{
			FVector Dir{ MoveDir.x(), MoveDir.y(), MoveDir.z()};
			Dir.Normalize();
			AddMovementInput(Dir);
		}
	}
	else
	{
		// 안움직이면 최신 정보와 강제 동기화
		// 더미가 아닐 경우에만 
		if ( bIsDummy)
			return;

		FVector Location{ DestnInfo.pos().x(), DestnInfo.pos().y(), DestnInfo.pos().z() };
		SetActorLocation(Location);
		SetActorRotation(FRotator(0, DestnInfo.yaw(), 0));
	}
}
