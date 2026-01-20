// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyState_AttackSlowRate.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API UNotifyState_AttackSlowRate : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation) override;

private:
    float SlowRate = 0.25f;
};
