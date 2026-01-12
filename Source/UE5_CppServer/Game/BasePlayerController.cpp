// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BasePlayerController.h"
#include "Tags.h"
#include "PlayerBase.h"

void ABasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	BasePlayer = Cast<APlayerBase>(aPawn);
	check(BasePlayer);
}

void ABasePlayerController::HandleEvent(FGameplayTag EventTag)
{
	if (EventTag.MatchesTag(EventTags::Event_SetActionState_None))
	{
		BasePlayer->SetActionState(Protocol::ACTION_STATE_NONE);
	}
}
