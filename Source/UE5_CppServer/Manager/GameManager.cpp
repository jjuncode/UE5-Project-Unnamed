// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/GameManager.h"
#include "ClientPlayer.h"
#include "PlayerBase.h"

UGameManager::UGameManager()
	:Super()
{
	static ConstructorHelpers::FClassFinder<AActor> PlayerBPClass(TEXT("/Game/Blueprints/Object/BP_PlayerBase"));
	check(PlayerBPClass.Succeeded())

	OtherPlayerClass = PlayerBPClass.Class;
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
		// GameMode -> Default Pawn 이용한 spawn
		AClientPlayer* ClientPlayer = Cast<AClientPlayer>(World->GetFirstPlayerController()->GetCharacter());
		if (ClientPlayer == nullptr)
			return;
	
		MyPlayer = ClientPlayer;
		MyPlayer->SetMyPlayer();

		MyPlayer->SetActorLocation(SpawnLocation);
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
	Player->SetDesntInfo(MovePkt.player_info());								// 목적지 설정
	Player->SetMoveState(MovePkt.player_info().creature_info().move_state());	// 상태 설정
	Player->SetMoveDir(MovePkt.move_dir());										// 방향 설정
}

void UGameManager::HandleSkill(const Protocol::S_SKILL& SkillPkt)
{
	Protocol::ObjectInfo ObjectInfo = SkillPkt.object_info();

	// 스킬 쓴 애 정보 셋팅 
	TObjectPtr<APlayerBase>* SkillUsePlayer = Players.Find(ObjectInfo.creature_info().id());
	ensureMsgf(SkillUsePlayer, TEXT("[GameManager - HandleSkill] : Can't Find Player"));

	(*SkillUsePlayer)->SetObjectInfo(ObjectInfo);
}

void UGameManager::HandleDebugMessage(const Protocol::S_DEBUG& DebugPkt)
{
	// debug Message Rendering
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	FColor color;
	switch (DebugPkt.info().color())
	{
	case 0:
		color = FColor::Red;
		break;
	case 1:
		color = FColor::Green;
		break;
	case 2:
		color = FColor::Blue;
		break;
	default:
		break;
	}

	FVector center(DebugPkt.info().center().x(), DebugPkt.info().center().y(), DebugPkt.info().center().z());
	FVector extent(DebugPkt.info().radius().x(), DebugPkt.info().radius().y(), DebugPkt.info().radius().z());
	float duration = DebugPkt.info().duration();

	// Shape 구분
	switch (DebugPkt.info().shape()) // 예: 0=Box, 1=Sphere, 2=Line
	{
	case Protocol::DEBUG_SHAPE_BOX: // Box
		DrawDebugBox(World, center, extent, FQuat::Identity, color, false, duration, 0, 5.0f);
		break;

	case Protocol::DEBUG_SHAPE_CIRCLE: // Sphere
		DrawDebugCircle(World, center, extent.X, 16, color, false, duration, 0, 5.0f);
		break;

	case Protocol::DEBUG_SHAPE_LINE: // Line
	{
		// center = start
		FVector start = center;
		FVector end = center + extent;

		DrawDebugLine(World, start, end, color, false, duration, 0, 5.0f);
	}
	break;

	default: // Box fallback
		DrawDebugBox(World, center, extent, FQuat::Identity, color, false, duration, 0, 5.0f);
		break;
	}
}

bool UGameManager::IsMyPlayer(TObjectPtr<class APlayerBase> rhs)
{
	return IsMyPlayer(rhs->GetObjectInfo());
}

bool UGameManager::IsMyPlayer(int32 Id)
{
	if (Id == MyPlayer->GetObjectInfo().creature_info().id())
		return true;
	return false;
}

bool UGameManager::IsMyPlayer(const Protocol::ObjectInfo& ObjectInfo)
{
	return IsMyPlayer(ObjectInfo.creature_info().id());
}
