// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SlowEffect_Start.generated.h"

/**
 * 
 */
UCLASS()
class UE5_CPPSERVER_API UAnimNotify_SlowEffect_Start : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = PlayRate)
	float PlayRate = 0.03125f;
};
