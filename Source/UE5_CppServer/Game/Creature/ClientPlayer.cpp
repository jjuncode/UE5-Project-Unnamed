// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "NetUtils.h"
#include "ServerPacketHandler.h"
#include "UE5_CppServer.h"
#include "ClientPlayerController.h"

AClientPlayer::AClientPlayer()
	:Super()
{
	// ===== Camera Pivot =====
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot")); 
	CameraPivot->SetupAttachment(RootComponent);      
	CameraPivot->SetRelativeLocation(FVector::ZeroVector);
	CameraPivot->SetRelativeRotation(FRotator::ZeroRotator);

	// ===== SpringArm =====
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CameraPivot);
	SpringArm->TargetArmLength = 415.f;
	SpringArm->bUsePawnControlRotation = false;   // 컨트롤러 회전 사용 X 

	// ===== Camera =====
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // 카메라는 SpringArm만 따라감

	// ===============================
	// Camera State 초기값
	// ===============================
	// Normal
	{
		FCameraStateData State;
		State.PivotOffset = FVector::ZeroVector;
		State.PivotRotation = FRotator(-30.f, 0.f, 0.f);
		State.ArmLength = 400.f;
		State.InterpSpeed = 10.f;
		CameraStates.Add(ECameraState::Normal, State);
	}

	// Battle
	{
		FCameraStateData State;
		State.PivotOffset = FVector(0.f, 70.f, 80.f);
		State.PivotRotation = FRotator(-20.f, 0.f, 0.f);
		State.ArmLength = 100.f;
		State.InterpSpeed = 10.f;
		CameraStates.Add(ECameraState::Battle, State);
	}

	// Parry (측면)
	{
		FCameraStateData State;
		State.PivotOffset = FVector(0.f, 70.f, 80.f);
		State.PivotRotation = FRotator(-30.f, -45.f, 0.f);
		State.ArmLength = 100.f;
		State.InterpSpeed = 15.f;
		CameraStates.Add(ECameraState::Parry, State);
	}
}

void AClientPlayer::HandleEvent(FGameplayTag EventTag)
{
	Controller->HandleEvent(EventTag);
}

void AClientPlayer::MoveSync(float DeltaTime)
{
	// Send Move Packet
	constexpr float SendPacketTime = 0.25f;
	static float AccTime = 0;
	AccTime += DeltaTime;

	if (bForceSendMovePkt || AccTime >= SendPacketTime)
	{
		Protocol::C_MOVE MovePkt;
		Protocol::ObjectInfo* PlayerInfo = MovePkt.mutable_player_info();
		PlayerInfo->CopyFrom(GetObjectInfo());

		Protocol::Vec3* Dir = MovePkt.mutable_move_dir();
		Dir->CopyFrom(MoveDir);

		 SEND_PACKET_NO_SESSION(MovePkt);
	}

	// 자신은 자신이 움직이니까 상관없다.
	DestnInfo = ObjectInfo;
}

void AClientPlayer::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessAI = EAutoPossessAI::Disabled;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

}

void AClientPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSync(DeltaTime);
	UpdateCamera(DeltaTime);
}

void AClientPlayer::Caching()
{
	bIsMyPlayer = true;

	// Controller 셋팅
	{
		Controller = Cast<AClientPlayerController>(GetController());
		check(Controller);
	}
}

void AClientPlayer::SetCameraState(ECameraState NewState)
{
	if (CurCameraState == NewState)
		return;

	CurCameraState = NewState;

	// Controller 회전값 초기화 
	Controller->SetControlRotation(GetActorRotation());
}

void AClientPlayer::UpdateCamera(float DeltaTime)
{
	const FCameraStateData* TargetState = CameraStates.Find(CurCameraState);
	if (!TargetState)
		return;	

	// Pivot 위치
	CameraPivot->SetRelativeLocation(
		FMath::VInterpTo(
			CameraPivot->GetRelativeLocation(),
			TargetState->PivotOffset,
			DeltaTime,
			TargetState->InterpSpeed
		)
	);

	// SpringArm회전
	// 피봇 회전은 그냥 offset값으로 하는것 
	SpringArm->SetWorldRotation(
		FMath::RInterpTo(
			SpringArm->GetComponentRotation(),
			GetControlRotation() + TargetState->PivotRotation,
			DeltaTime,
			TargetState->InterpSpeed
		)
	);

	// SpringArm 거리
	SpringArm->TargetArmLength =
		FMath::FInterpTo(
			SpringArm->TargetArmLength,
			TargetState->ArmLength,
			DeltaTime,
			TargetState->InterpSpeed
		);
}
