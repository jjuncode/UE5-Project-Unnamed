// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Creature.h"

// Sets default values
ACreature::ACreature()
	:Super()
{
}

void ACreature::BeginPlay()
{
	Super::BeginPlay();

	// Idle
	{
		Protocol::CreatureInfo* CreatureInfo = ObjectInfo.mutable_creature_info();
		CreatureInfo->set_state(Protocol::MOVE_STATE_IDLE);
	}

	// 정보 동기화
	{
		auto Location = GetActorLocation();
		auto Rotation = GetActorRotation();

		Protocol::ObjectInfo ObjInfo = GetObjectInfo();

		Protocol::Pos* pos = ObjInfo.mutable_pos();
		pos->set_x(Location.X);
		pos->set_y(Location.Y);
		pos->set_z(Location.Z);

		ObjInfo.set_yaw(Rotation.Yaw);

		SetObjectInfo(ObjInfo);
		SetDesntInfo(ObjInfo);
	}
}

void ACreature::Tick(float DeltaTime)
{
	// 정보 동기화
	{
		auto Location = GetActorLocation();
		auto Rotation = GetActorRotation();

		Protocol::ObjectInfo ObjInfo = GetObjectInfo();

		Protocol::Pos* pos = ObjInfo.mutable_pos();
		pos->set_x(Location.X);
		pos->set_y(Location.Y);
		pos->set_z(Location.Z);

		ObjInfo.set_yaw(Rotation.Yaw);

		SetObjectInfo(ObjInfo);
	}

}

void ACreature::SetMoveState(const Protocol::MoveState& rhs)
{
	Protocol::CreatureInfo* CreatureInfo = ObjectInfo.mutable_creature_info();
	CreatureInfo->set_state(rhs);
}