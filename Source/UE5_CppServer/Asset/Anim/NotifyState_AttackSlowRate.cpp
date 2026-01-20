// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/NotifyState_AttackSlowRate.h"

void UNotifyState_AttackSlowRate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (auto* AnimInst = MeshComp->GetAnimInstance())
    {
        AnimInst->Montage_SetPlayRate(nullptr, SlowRate);
        // nullptr → 현재 재생 중 Montage
    }
}

void UNotifyState_AttackSlowRate::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (auto* AnimInst = MeshComp->GetAnimInstance())
    {
        AnimInst->Montage_SetPlayRate(nullptr, 1.0f);
    }
}