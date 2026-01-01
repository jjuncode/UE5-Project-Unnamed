// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerBase.h"

APlayerBase::APlayerBase()
	:Super()
{
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ObjectInfo.creature_info().state() == Protocol::MOVE_STATE_RUN)
	{
		// Rotate
		{
			FRotator Rotator = GetController()->GetControlRotation();
			Rotator.Yaw = DestnInfo.yaw();

			GetController()->SetControlRotation(Rotator);
		}

		// Move
		{
			FVector DestnPos = { DestnInfo.pos().x(),  DestnInfo.pos().y(),  DestnInfo.pos().z() };
			FVector CurPos = { ObjectInfo.pos().x(),  ObjectInfo.pos().y(),  ObjectInfo.pos().z() };


			FVector dir = DestnPos - CurPos;
			dir.Normalize();
			AddMovementInput(dir);
		}
	}
}
