// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/AnimNotify_SlowEffect_Start.h"
#include "Creature.h"
#include "Parryable.h"
#include "PlayerAnimInstance.h"

void UAnimNotify_SlowEffect_Start::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (auto* AnimInst = MeshComp->GetAnimInstance())
    {
		// 패링상태면 슬로우를 조금만 느리게해줌 
		ACreature* Creature = Cast<ACreature>(MeshComp->GetOwner());
		if (Creature)
		{
			IParryable* ParryCreature = Cast<IParryable>(Creature);
			if (ParryCreature && Creature->GetActionState() == Protocol::ACTION_STATE_PARRY)
			{
				// 패링 상태면 슬로우 안함 
				return;
			}
		}

        // nullptr → 현재 재생 중 Montage
        AnimInst->Montage_SetPlayRate(nullptr, PlayRate);
    }
}
