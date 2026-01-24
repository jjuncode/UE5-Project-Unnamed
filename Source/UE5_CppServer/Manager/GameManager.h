// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetUtils.h"
#include "Protocol.pb.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGameManager();

public:
	void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine, bool IsDummy);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);
	void HandleSpawn(const Protocol::S_SPAWNDUMMY& SpawnDummyPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleSkill(const Protocol::S_SKILL& SkillPkt);
	void HandleDamaged(const Protocol::S_DAMAGED& DamagePkt);
	void HandleParry(const Protocol::S_PARRY& ParryPkt);
	void HandleSelectEnemy(const Protocol::S_SELECT_ENEMY& SelectEnemyPkt);

	void HandleDebugMessage(const Protocol::S_DEBUG& DebugPkt);

	bool IsMyPlayer(TObjectPtr<class APlayerBase> rhs);
	bool IsMyPlayer(int32 Id);
	bool IsMyPlayer(const Protocol::ObjectInfo& ObjectInfo);

	const auto& GetPlayers() { return Players;  }
	TObjectPtr<class APlayerBase> GetPlayer(uint64 PlayerId);

protected:
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AActor> OtherPlayerClass;

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AActor> DummyPlayerClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AClientPlayer> MyPlayer;
	UPROPERTY()
	TMap<uint64, TObjectPtr<class APlayerBase>> Players; // 전체 목록 ( My포함 )  
};
