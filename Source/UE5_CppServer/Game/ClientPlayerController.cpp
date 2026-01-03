// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ClientPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "ClientPlayer.h"

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
}

void AClientPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Bind IA
	{
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
		check(EIC);

		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMoveActionTrigerred);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMoveActionCompleted);
	
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMouseLookAction);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMouseLookAction);
	}
}

void AClientPlayerController::_HandleMoveAction(const FInputActionValue& Value, const Protocol::MoveState& State, AClientPlayer* ClientPlayer)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	auto Forward = ClientPlayer->GetActorForwardVector() * InputValue.X;
	auto Right = ClientPlayer->GetActorRightVector() * InputValue.Y;

	// 이동방향 셋팅
	FVector MoveDir = Forward + Right;
	ClientPlayer->SetMoveDir(MoveDir);

	ClientPlayer->AddMovementInput(MoveDir);
	ClientPlayer->SetMoveState(State);
}

void AClientPlayerController::HandleMoveActionTrigerred(const FInputActionValue& Value)
{
	AClientPlayer* ClientPlayer = Cast<AClientPlayer>(GetCharacter());
	check(ClientPlayer);

	_HandleMoveAction(Value, Protocol::MOVE_STATE_RUN, ClientPlayer);
}

void AClientPlayerController::HandleMoveActionCompleted(const FInputActionValue& Value)
{
	AClientPlayer* ClientPlayer = Cast<AClientPlayer>(GetCharacter());
	check(ClientPlayer);
	ClientPlayer->ForceSendMovePkt();

	_HandleMoveAction(Value, Protocol::MOVE_STATE_IDLE, ClientPlayer);
}


void AClientPlayerController::HandleMouseLookAction(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	AddYawInput(InputValue.X);
	AddPitchInput(InputValue.Y);

	AClientPlayer* ClientPlayer = Cast<AClientPlayer>(GetCharacter());
}
