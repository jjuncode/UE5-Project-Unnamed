// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NativeGameplayTags.h"

namespace ActionState
{
    // Action State
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_None);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Move_Idle);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Move_Run);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Battle);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Attack_Try);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Attack_Success);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Attack_Interrupted);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Parry);
}

namespace EventTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_SetActionState_None);
}