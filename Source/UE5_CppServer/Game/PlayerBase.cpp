// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/PlayerBase.h"
#include "ClientPlayer.h"
#include "BasePlayerController.h"

void APlayerBase::HandleEvent(FGameplayTag EventTag)
{
	Controller->HandleEvent(EventTag);
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveSync();
}

void APlayerBase::Caching()
{
	bIsMyPlayer = false;

	// Set Controller
	Controller = Cast<ABasePlayerController>(GetController());
	check(Controller);
}

void APlayerBase::MoveSync()
{
	// ClientPlayer는 제외
	if (bIsMyPlayer)
		return;

	if (ObjectInfo.creature_info().move_state() == Protocol::MOVE_STATE_RUN)
	{
		// Rotate
		{
			SetActorRotation(FRotator(0, DestnInfo.yaw(), 0));
		}

		// Move
		{
			FVector Dir{ MoveDir.x(), MoveDir.y(), MoveDir.z()};
			Dir.Normalize();
			AddMovementInput(Dir);
		}
	}
	else
	{
		// 안움직이면 최신 정보와 강제 동기화
		FVector Location{ DestnInfo.pos().x(), DestnInfo.pos().y(), DestnInfo.pos().z() };
		SetActorLocation(Location);
		SetActorRotation(FRotator(0, DestnInfo.yaw(), 0));
	}
}
