// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientPlayer.h"
#include "NetUtils.h"
#include "ServerPacketHandler.h"
#include "UE5_CppServer.h"
#include "ClientPlayerController.h"

void AClientPlayer::HandleEvent(FGameplayTag EventTag)
{
	Controller->HandleEvent(EventTag);
}

void AClientPlayer::MoveSync(float DeltaTime)
{
	// Send Move Packet
	constexpr float SendPacketTime = 0.25f;
	static float AccTime = 0;
	AccTime += DeltaTime;

	if (bForceSendMovePkt || AccTime >= SendPacketTime)
	{
		Protocol::C_MOVE MovePkt;
		Protocol::ObjectInfo* PlayerInfo = MovePkt.mutable_player_info();
		PlayerInfo->CopyFrom(GetObjectInfo());

		Protocol::Vec3* Dir = MovePkt.mutable_move_dir();
		Dir->CopyFrom(MoveDir);

		 SEND_PACKET_NO_SESSION(MovePkt);
	}

	// 자신은 자신이 움직이니까 상관없다.
	DestnInfo = ObjectInfo;
}

void AClientPlayer::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AClientPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSync(DeltaTime);
}

void AClientPlayer::Caching()
{
	bIsMyPlayer = true;

	// Controller 셋팅
	{
		Controller = Cast<AClientPlayerController>(GetController());
		check(Controller);
	}
}