// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/GameManager.h"
#include "ClientPlayer.h"
#include "PlayerBase.h"

UGameManager::UGameManager()
	:Super()
{
	static ConstructorHelpers::FClassFinder<AActor> PlayerBPClass(TEXT("/Game/Blueprints/BP_PlayerBase"));
	if (PlayerBPClass.Succeeded())
	{
		OtherPlayerClass = PlayerBPClass.Class;
	}
}

void UGameManager::HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = PlayerInfo.creature_info().id();

	// 이미 있는지 체크
	if (Players.Find(ObjectId) != nullptr)
		return;

	FVector SpawnLocation{ PlayerInfo.pos().x(), PlayerInfo.pos().y(), PlayerInfo.pos().z() };

	if (IsMine)
	{
		// 기본 스폰 character 이용했음
		// TODO : ? 지우고 내가 스폰시키기
		AClientPlayer* ClientPlayer = Cast<AClientPlayer>(World->GetFirstPlayerController()->GetCharacter());
		if (ClientPlayer == nullptr)
			return;
	
		MyPlayer = ClientPlayer;
		MyPlayer->SetObjectInfo(PlayerInfo);
		Players.Add(ObjectId, ClientPlayer);
	}
	else
	{
		// Spawn Other
		AActor* SpawnPlayer = World->SpawnActor(OtherPlayerClass, &SpawnLocation);
		APlayerBase* Player = Cast<APlayerBase>(SpawnPlayer);
		check(Player);
		
		Player->SetObjectInfo(PlayerInfo);
		Players.Add(ObjectId, Player);
	}
}

void UGameManager::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	// sucess 체크 이미 했음
	HandleSpawn(EnterGamePkt.player_info(), true);
}

void UGameManager::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& Player : SpawnPkt.players_info())
		HandleSpawn(Player, false);
}

void UGameManager::HandleDespawn(uint64 ObjectId)
{
	// TODO : Despawn Logic
	TObjectPtr<APlayerBase>* FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;
	TObjectPtr<APlayerBase> Player = *FindActor;

	Players.Remove(ObjectId);

	if (Player->GetObjectInfo().creature_info().id() == ObjectId)
		MyPlayer = nullptr;

	Player->Destroy();
}

void UGameManager::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (auto& id : DespawnPkt.object_ids())
		HandleDespawn(id);
}

void UGameManager::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = MovePkt.player_info().creature_info().id();
	TObjectPtr<APlayerBase>* FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	TObjectPtr<APlayerBase> Player = *FindActor;

	if (IsMyPlayer(Player))
		return;

	// Only About Other Player
	//Player->SetObjectInfo(MovePkt.player_info());	// 정보 최신화
	Player->SetDesntInfo(MovePkt.player_info());							// 목적지 설정
	Player->SetMoveState(MovePkt.player_info().creature_info().state());	
}

bool UGameManager::IsMyPlayer(TObjectPtr<class APlayerBase> rhs)
{
	if (rhs->GetObjectInfo().creature_info().id() == MyPlayer->GetObjectInfo().creature_info().id())
		return true;
	return false;
}
