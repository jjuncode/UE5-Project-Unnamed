// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsClasses.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* aPawn) override;

public:
	// -----------------------
	//		Handle Event 
	// -----------------------
	void HandleEvent(FGameplayTag EventTag);

private:
	UPROPERTY(Category = Character, VisibleAnywhere)
	TObjectPtr<class APlayerBase> BasePlayer;
};
