// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/AnimNotify_SlowEffect_End.h"
#include "Creature.h"
#include "Parryable.h"
#include "PlayerAnimInstance.h"

void UAnimNotify_SlowEffect_End::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
  
    if (auto* AnimInst = MeshComp->GetAnimInstance())
    {
        // nullptr → 현재 재생 중 Montage
        AnimInst->Montage_SetPlayRate(nullptr, 1.0f);

		// 패링상태 검출
		ACreature* Creature = Cast<ACreature>(MeshComp->GetOwner());
		if (Creature)
		{
			IParryable* ParryCreature = Cast<IParryable>(Creature);
			if (ParryCreature && Creature->GetActionState() == Protocol::ACTION_STATE_PARRY )
			{
				// 패링 상태면 재생
				UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(AnimInst);
				if (PlayerAnim)
				{
					PlayerAnim->PlayParryTiming = true;
				}
			}
		}
    }
}
