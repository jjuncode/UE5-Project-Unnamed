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
	
private:
	void MoveSync(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override;
};
