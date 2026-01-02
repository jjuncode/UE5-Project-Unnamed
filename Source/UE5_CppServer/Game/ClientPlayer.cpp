// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ClientPlayer.h"
#include "NetUtils.h"

void AClientPlayer::MoveSync(float DeltaTime)
{
	constexpr float SendPacketTime = 2.5f;

	static float AccTime = 0;

	AccTime += DeltaTime;

	if (AccTime >= SendPacketTime)
	{
		Protocol::C_MOVE MovePkt;
		Protocol::ObjectInfo* PlayerInfo = MovePkt.mutable_player_info();
		PlayerInfo->CopyFrom(GetObjectInfo());

		// SEND_PACKET_NO_SESSION(MovePkt);
	}
}

void AClientPlayer::Tick(float DeltaTime)
{
	MoveSync(DeltaTime);
}
