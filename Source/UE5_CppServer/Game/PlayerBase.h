// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "PlayerBase.generated.h"

UCLASS()
class UE5_CPPSERVER_API APlayerBase : public ACreature
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Caching();

	void SetCurPlayingSkill(const Protocol::SkillInfo& Info) { ObjectInfo.mutable_creature_info()->set_skill_info(Info); }
	Protocol::SkillInfo GetCurPlayingSkill() { return  ObjectInfo.creature_info().skill_info(); }

	// ----------------------
	//		  Battle
	// ----------------------
	void OnDamaged(const Protocol::S_DAMAGED& DamagePkt);

	Protocol::AttackDir GetDamagedDir() const { return DamagedDir; }
	void ResetDamageDir() { DamagedDir = Protocol::DIR_NONE; }

	// ----------------------
	//		Event Handle
	// ----------------------
	virtual void HandleEvent(FGameplayTag EventTag) override;

	// -------------------
	//		Network
	// -------------------
	void MoveSync();

protected:
	// Caching 
	TObjectPtr<class ABasePlayerController> Controller;
	Protocol::AttackDir DamagedDir;	// 데미지 받은 방향 

	bool bIsMyPlayer = false;
};
