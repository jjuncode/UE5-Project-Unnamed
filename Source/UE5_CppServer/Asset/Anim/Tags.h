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

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged_UP);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged_DOWN);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged_LEFT);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged_RIGHT);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_OnDamaged_FRONT);
}

namespace EventTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_SetActionState_None);
}