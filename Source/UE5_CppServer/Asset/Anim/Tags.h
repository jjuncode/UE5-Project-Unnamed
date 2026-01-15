// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NativeGameplayTags.h"

namespace StateTags
{
    // Move State
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Move_None);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Move_Idle);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Move_Run);

    // Action State
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_None);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Skill);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged);
}

namespace EventTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_SetActionState_None);
}