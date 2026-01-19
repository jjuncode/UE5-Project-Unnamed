// Fill out your copyright notice in the Description page of Project Settings.


#include "Tags.h"

namespace ActionState
{
	 // Action State
    UE_DEFINE_GAMEPLAY_TAG(State_Action_None        , "State.Action.None");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Move_Idle   , "State.Action.Move.Idle");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Move_Run    , "State.Action.Move.Run");
    
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Attack_Try          , "State.Action.Attack.Try");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Attack_Success      , "State.Action.Attack.Success");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Attack_Interrupted  , "State.Action.Attack.Interrupted");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_OnDamaged           , "State.Action.Attack.OnDamaged");
    UE_DEFINE_GAMEPLAY_TAG(State_Action_Parry               , "State.Action.Attack.Parry");
}

namespace EventTags
{
    UE_DEFINE_GAMEPLAY_TAG(Event_SetActionState_None, "Event.SetActionState.None");
}