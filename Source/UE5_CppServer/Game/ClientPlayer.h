// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/PlayerBase.h"
#include "ClientPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API AClientPlayer : public APlayerBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	
public:
	void SetMyPlayer() { bIsMyPlayer = true; }
	void ForceSendMovePkt() { bForceSendMovePkt = true; }

private:
	void MoveSync(float DeltaTime);

private:
	bool bForceSendMovePkt = false;
};
