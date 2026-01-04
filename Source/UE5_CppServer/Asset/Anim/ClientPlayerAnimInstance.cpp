// Fill out your copyright notice in the Description page of Project Settings.


#include "Asset/Anim/ClientPlayerAnimInstance.h"
#include "ClientPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UClientPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// ClientPlayer
	ClientPlayer = Cast<AClientPlayer>(TryGetPawnOwner());

	// CharacterMovement
	if ( ClientPlayer) 
		MovementComponent = ClientPlayer->GetCharacterMovement();
}

void UClientPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Ä³½Ì Àç½Ãµµ 
	if (ClientPlayer == nullptr)
	{
		ClientPlayer = Cast<AClientPlayer>(TryGetPawnOwner());

		// CharacterMovement
		if (ClientPlayer)
		{
			MovementComponent = ClientPlayer->GetCharacterMovement();
		}
		else
			return;
	}

	// Set Velocity & Ground Speed
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	// Setting bShouldMove
	FVector Accel = MovementComponent->GetCurrentAcceleration();
	if (GroundSpeed >= 0.01 && Accel.Length() != 0)
	{
		bShouldMove = true;
	}
	else
	{
		bShouldMove = false;
	}

	// Setting IsFalling
	bIsFalling = MovementComponent->IsFalling();

	// Calculate direction using the delta between the velocity and the actor rotation.
	// When the character is not strafing, clamp the value between - and + 45 degrees 
	// so that backwards animations do not play when turning around, 
	// but running into wall looks better.
	
	float ret = CalculateDirection(Velocity, ClientPlayer->GetActorRotation());

	if (MovementComponent->bOrientRotationToMovement)
	{
		Direction = FMath::Clamp(ret, -45.f, 45.f);
	}
	else
	{
		Direction = ret;
	}
}
