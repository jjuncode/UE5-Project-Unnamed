// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"

// Sets default values
ACreature::ACreature()
	:Super()
{
}

void ACreature::BeginPlay()
{
	Super::BeginPlay();

	// Idle
	SetActionState(Protocol::ACTION_STATE_MOVE_IDLE);

	// 정보 동기화
	{
		auto Location = GetActorLocation();
		auto Rotation = GetActorRotation();

		Protocol::ObjectInfo ObjInfo = GetObjectInfo();

		Protocol::Vec3* Pos = ObjInfo.mutable_pos();
		Pos->set_x(Location.X);
		Pos->set_y(Location.Y);
		Pos->set_z(Location.Z);

		ObjInfo.set_yaw(Rotation.Yaw);

		SetObjectInfo(ObjInfo);
		SetDesntInfo(ObjInfo);
	}
}

void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 정보 동기화
	{
		auto Location = GetActorLocation();
		auto Rotation = GetActorRotation();

		Protocol::ObjectInfo ObjInfo = GetObjectInfo();

		Protocol::Vec3* pos = ObjInfo.mutable_pos();
		pos->set_x(Location.X);
		pos->set_y(Location.Y);
		pos->set_z(Location.Z);

		ObjInfo.set_yaw(Rotation.Yaw);

		SetObjectInfo(ObjInfo);
	}
}

void ACreature::SetActionState(const Protocol::ActionState& rhs)
{
	Protocol::CreatureInfo* CreatureInfo = ObjectInfo.mutable_creature_info();
	CreatureInfo->set_action_state(rhs);
}

void ACreature::HandleEvent(FGameplayTag EventTag)
{
	// 각자 override
}

void ACreature::SetMoveDir(const FVector& Vec)
{
	MoveDir.set_x(Vec.X);
	MoveDir.set_y(Vec.Y);
	MoveDir.set_z(Vec.Z);
};