// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "ClientPlayer.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Normal,
	Battle,
	Parry,
};

struct FCameraStateData
{
	FVector PivotOffset = FVector::ZeroVector;
	FRotator PivotRotation = FRotator::ZeroRotator;
	float ArmLength = 415.f;
	float InterpSpeed = 8.f;
};

UCLASS()
class UE5_CPPSERVER_API AClientPlayer : public APlayerBase
{
	GENERATED_BODY()

public:
	AClientPlayer();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	void Caching();

	// ----------------------
	//		Battle
	// ----------------------
	void SetTarget(ACreature* Creature) { Target = Creature; }
	void ResetTarget() { Target = nullptr; }
	TObjectPtr<class ACreature> GetTarget() { return Target; }

	// --------------------------------
	//		Interface - Damageble
	// --------------------------------
	virtual void OnDamaged(const Protocol::S_DAMAGED& DamagePkt) override;

	// --------------------------------
	//		Interface - Parryable
	// --------------------------------
	virtual void Parry(Protocol::S_PARRY ParryInfo);

	// ----------------------
	//		Camera
	// ----------------------
public:
	void SetCameraState(ECameraState NewState);
	FVector GetCameraForward();

private:
	void UpdateCamera(float DeltaTime);

	// ----------------------
	//		Handle Event
	// ----------------------
public:
	virtual void HandleEvent(FGameplayTag EventTag) override;

	// -------------------
	//		Network
	// -------------------
public:
	void ForceSendMovePkt() { bForceSendMovePkt = true; }

private:
	void MoveSync(float DeltaTime);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USceneComponent> CameraPivot;

protected:
	// Camera
	TMap<ECameraState, FCameraStateData> CameraStates;
	
	UPROPERTY(EditAnywhere, Category = "Camera State")
	ECameraState CurCameraState = ECameraState::Normal;

private:
	// Caching
	TObjectPtr<class AClientPlayerController> Controller;
	TObjectPtr<class ACreature> Target;

	// Network
	bool bForceSendMovePkt = false;
};
