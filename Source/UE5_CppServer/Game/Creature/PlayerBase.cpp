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

void APlayerBase::MoveSync()
{
	// ClientPlayer는 제외
	if (bIsMyPlayer)
		return;

	if (ObjectInfo.creature_info().move_state() == Protocol::MOVE_STATE_RUN)
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
		FVector Location{ DestnInfo.pos().x(), DestnInfo.pos().y(), DestnInfo.pos().z() };
		SetActorLocation(Location);
		SetActorRotation(FRotator(0, DestnInfo.yaw(), 0));
	}
}
