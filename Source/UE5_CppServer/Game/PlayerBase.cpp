// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/PlayerBase.h"
#include "ClientPlayer.h"

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ObjectInfo.creature_info().state() == Protocol::MOVE_STATE_RUN)
	{
		// ClientPlayer´Â Á¦¿Ü
		if (auto* ClientPlayer = Cast<AClientPlayer>(this))
			return;

		// Rotate
		{
			SetActorRotation(FRotator(0, DestnInfo.yaw(),0));
		}

		// Move
		{
			AddMovementInput(GetActorForwardVector());
		}
	}
}
