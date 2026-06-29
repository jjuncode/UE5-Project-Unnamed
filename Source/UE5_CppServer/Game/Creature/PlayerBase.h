// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"

#include "Interface/Weaponable.h"
#include "Interface/Damageable.h"
#include "Interface/Parryable.h"

#include "PlayerBase.generated.h"

UCLASS()
class UE5_CPPSERVER_API APlayerBase : public ACreature, public IWeaponable, public IDamageable, public IParryable
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Caching(bool IsDummy = false);

	void SetCurPlayingSkill(const Protocol::SkillId& Info) { ObjectInfo.mutable_creature_info()->set_skill_info(Info); }
	Protocol::SkillId GetCurPlayingSkill() { return  ObjectInfo.creature_info().skill_info(); }

	// --------------------------------
	//		Interface - Weaponable
	// --------------------------------
	UFUNCTION(BlueprintCallable)
	virtual void AttachWeapon(TSubclassOf<class AWeapon> WeaponInfo) override;

	// --------------------------------
	//		Interface - Damageble
	// --------------------------------
	virtual void OnDamaged(const Protocol::S_DAMAGED& DamagePkt) override;

	virtual Protocol::AttackDir GetDamagedDir() const override { return DamagedDir; }
	virtual void ResetDamageDir() override { DamagedDir = Protocol::DIR_NONE; }

	// --------------------------------
	//		Interface - Parryable
	// --------------------------------
	virtual void Parry();
	virtual void CachingParryAttackInfo(Protocol::S_PARRY ParryInfo);	// 패링정보 캐싱

	// ----------------------
	//		Event Handle
	// ----------------------
	virtual void HandleEvent(FGameplayTag EventTag) override;

	// -------------------
	//		Network
	// -------------------
	void MoveSync();
	bool IsDummy() { return bIsDummy; }

	//새로 추가
	bool IsMoveState() const;
	bool CanMove() const;
	bool CanUseSkill() const;
	bool CanDash() const;
	bool CanDodge() const;
	bool CanAttack() const;
	bool CanParry() const;
	bool CanTakeDamage() const;
	bool IsDeadState() const;
	bool CanDie() const;
	bool IsGroggyState() const;
	bool CanRecoverFromGroggy() const;
	bool IsActionBlockedState() const;

protected:
	// Caching 
	UPROPERTY(EditAnyWhere, Category = "Weapon")
	TSubclassOf<class AWeapon> Weapon;

	TObjectPtr<class ABasePlayerController> Controller;
	Protocol::AttackDir DamagedDir;	// 데미지 받은 방향 

	// 패링 정보 캐싱 해둠 ( 애니메이션이 재생될 때 수행되기 위한 ) 
	Protocol::S_PARRY ParryAttackInfo;

	bool bIsMyPlayer = false;
	bool bIsDummy = false;
};
