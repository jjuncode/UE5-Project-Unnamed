// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ClientPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

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

		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMoveAction);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMoveAction);
	
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMouseLookAction);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMouseLookAction);
	}
}

void AClientPlayerController::HandleMoveAction(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	auto* ClientPlayer = GetCharacter();
	check(ClientPlayer);

	auto Forward = ClientPlayer->GetActorForwardVector() * InputValue.X;
	auto Right = ClientPlayer->GetActorRightVector() * InputValue.Y;

	ClientPlayer->AddMovementInput(Forward + Right);
}

void AClientPlayerController::HandleMouseLookAction(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	AddYawInput(InputValue.X);
	AddPitchInput(InputValue.Y);
}
