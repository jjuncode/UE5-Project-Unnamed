// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "PlayerBase.generated.h"

UCLASS()
class UE5_CPPSERVER_API APlayerBase : public ACreature
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Caching();

	// ----------------------
	//		Event Handle
	// ----------------------
	virtual void HandleEvent(FGameplayTag EventTag) override;

	// -------------------
	//		Network
	// -------------------
	void MoveSync();

protected:
	// Caching 
	TObjectPtr<class ABasePlayerController> Controller;

	bool bIsMyPlayer = false;
};
