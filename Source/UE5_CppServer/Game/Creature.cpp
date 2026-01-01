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

	Protocol::Pos* pos = DestnInfo.mutable_pos();
	pos->CopyFrom(ObjectInfo.pos());

	// Idle
	Protocol::CreatureInfo* CreatureInfo = ObjectInfo.mutable_creature_info();
	CreatureInfo->set_state(Protocol::MOVE_STATE_IDLE);
}

void ACreature::SetMoveState(const Protocol::MoveState& rhs)
{
	Protocol::CreatureInfo* CreatureInfo = ObjectInfo.mutable_creature_info();
	CreatureInfo->set_state(rhs);
}