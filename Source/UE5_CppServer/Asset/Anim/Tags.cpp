// Fill out your copyright notice in the Description page of Project Settings.


#include "Tags.h"

namespace StateTags
{
    UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Walking, "Movement.Mode.Walking");

    // Move State
    UE_DEFINE_GAMEPLAY_TAG(State_Move_None , "State.Move.None");
    UE_DEFINE_GAMEPLAY_TAG(State_Move_Idle , "State.Move.None");
    UE_DEFINE_GAMEPLAY_TAG(State_Move_Run  , "State.Move.None");
    
     // Action State
     UE_DEFINE_GAMEPLAY_TAG(State_Action_None       , "State.Action.None");
     UE_DEFINE_GAMEPLAY_TAG(State_Action_Skill      , "State.Action.Skill");

     UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged_UP, "State.Action.OnDamaged.Up");
     UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged_DOWN, "State.Action.OnDamaged.Down");
     UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged_LEFT, "State.Action.OnDamaged.Left");
     UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged_RIGHT, "State.Action.OnDamaged.Right");
     UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged_FRONT, "State.Action.OnDamaged.Front");
}

namespace EventTags
{
    UE_DEFINE_GAMEPLAY_TAG(Event_SetActionState_None, "Event.SetActionState.None");
}