// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

#include "ClientPlayer.h"
#include "NetUtils.h"
#include "ServerPacketHandler.h"
#include "UE5_CppServer.h"
#include "Tags.h"

void AClientPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add IMC to EnhancedInputCoimponent
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		check(LocalPlayer);

		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(Subsystem);
	
		Subsystem->AddMappingContext(IMC, 0);
	}

	ClientPlayer = Cast<AClientPlayer>(GetCharacter());
	check(ClientPlayer);
}

void AClientPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Bind IA
	{
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		check(EIC);

		// MOVE
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMoveActionTrigerred);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMoveActionCompleted);
	
		// Mouse Look
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMouseLookAction);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMouseLookAction);
	
		// Skill 
		EIC->BindAction(SkillAction, ETriggerEvent::Started, this, &AClientPlayerController::HandleSkillAction);
	}
}

void AClientPlayerController::HandleEvent(FGameplayTag EventTag)
{
	if (EventTag.MatchesTagExact(EventTags::Event_SetActionState_None))
	{
		ClientPlayer->SetActionState(Protocol::ACTION_STATE_NONE);
	}
}

void AClientPlayerController::_HandleMoveAction(const FInputActionValue& Value, const Protocol::ActionState& State )
{
	FVector2D InputValue = Value.Get<FVector2D>();

	auto Forward = ClientPlayer->GetActorForwardVector() * InputValue.X;
	auto Right = ClientPlayer->GetActorRightVector() * InputValue.Y;

	// 이동방향 셋팅
	FVector MoveDir = Forward + Right;
	ClientPlayer->SetMoveDir(MoveDir);

	ClientPlayer->AddMovementInput(MoveDir);
	ClientPlayer->SetActionState(State);
}

void AClientPlayerController::HandleMoveActionTrigerred(const FInputActionValue& Value)
{
	// IDLE / RUN 상태에서만 움직이기가 가능하다
	if (ClientPlayer->GetActionState() == Protocol::ACTION_STATE_MOVE_IDLE
		|| ClientPlayer->GetActionState() == Protocol::ACTION_STATE_MOVE_RUN)
	{}
	else return;

	_HandleMoveAction(Value, Protocol::ACTION_STATE_MOVE_RUN);
}

void AClientPlayerController::HandleMoveActionCompleted(const FInputActionValue& Value)
{
	ClientPlayer->ForceSendMovePkt();
	_HandleMoveAction(Value, Protocol::ACTION_STATE_MOVE_IDLE);
}


void AClientPlayerController::HandleMouseLookAction(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	AddYawInput(InputValue.X);
	AddPitchInput(InputValue.Y);
}

void AClientPlayerController::HandleSkillAction(const FInputActionValue& Value)
{
	if (ClientPlayer->GetActionState() == Protocol::ACTION_STATE_MOVE_IDLE
		|| ClientPlayer->GetActionState() == Protocol::ACTION_STATE_MOVE_RUN)
	{}
	else
		return;

	if (ClientPlayer->GetCurPlayingSkill() != Protocol::SKILL_INFO_NONE)
		return;

	const int SkillIndex = Value.Get<float>();

	Protocol::C_SKILL SkillPkt;

	// 스킬 idx 설정 
	Protocol::SkillInfo SkillInfo = static_cast<Protocol::SkillInfo>(SkillIndex);
	SkillPkt.mutable_skill_data()->set_id(SkillInfo);

	// 위치, yaw설정 
	Protocol::Vec3* Pos = SkillPkt.mutable_pos();
	FVector Location = ClientPlayer->GetActorLocation();
	Pos->set_x(Location.X);
	Pos->set_y(Location.Y);
	Pos->set_z(Location.Z);
	SkillPkt.set_yaw(ClientPlayer->GetObjectInfo().yaw());

	SEND_PACKET_NO_SESSION(SkillPkt);
}
