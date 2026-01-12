// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/AnimNotify_SendEvent.h"
#include "Creature.h"

void UAnimNotify_SendEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ACreature* Creature = Cast<ACreature>(MeshComp->GetOwner());
	if (Creature)
	{
		Creature->HandleEvent(EventTag);
	}
}
