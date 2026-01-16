// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "ClientPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API AClientPlayer : public APlayerBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	void Caching();

	// ----------------------
	//		Handle Event
	// ----------------------
public:
	virtual void HandleEvent(FGameplayTag EventTag) override;

	// -------------------
	//		Network
	// -------------------
public:
	void ForceSendMovePkt() { bForceSendMovePkt = true; }

private:
	void MoveSync(float DeltaTime);

private:
	// Caching
	TObjectPtr<class AClientPlayerController> Controller;

	// Network
	bool bForceSendMovePkt = false;
};
