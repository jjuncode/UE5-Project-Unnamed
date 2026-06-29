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

	// Parry 
	{
		FCameraStateData State;
		State.PivotOffset = FVector(0.f, 70.f, -15.f);
		State.PivotRotation = FRotator(2.5f, -10.f, 0.f);
		State.ArmLength = 130.f;
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

void AClientPlayer::OnDamaged(const Protocol::S_DAMAGED& DamagePkt)
{
	Super::OnDamaged(DamagePkt);

	if (Target == nullptr)
	{
		Target = GetGameManager()->GetPlayer(DamagePkt.attacker_id());
	}
}

void AClientPlayer::Parry()
{
	Super::Parry();
	SetCameraState(ECameraState::Parry);
}

void AClientPlayer::SetCameraState(ECameraState NewState)
{
	if (CurCameraState == NewState)
		return;

	CurCameraState = NewState;

	// Controller 회전값 초기화 
	Controller->SetControlRotation(GetActorRotation());
}

FVector AClientPlayer::GetCameraForward()
{
	return SpringArm->GetForwardVector();
}

bool AClientPlayer::ShouldLockOnTarget() const
{
	const Protocol::ActionState ActionState = GetActionState();

	return ActionState == Protocol::ACTION_STATE_ATTACK_TRY
		|| ActionState == Protocol::ACTION_STATE_ATTACK_SUCCESS
		|| ActionState == Protocol::ACTION_STATE_ATTACK_INTERRUPTED
		|| ActionState == Protocol::ACTION_STATE_DAMAGED
		|| ActionState == Protocol::ACTION_STATE_PARRY;
}

void AClientPlayer::ApplyLockOnRotation(float DeltaTime)
{
	if (Target == nullptr)
		return;

	const FVector TargetVec = Target->GetActorLocation() - GetActorLocation();

	FRotator TargetYawRot = TargetVec.Rotation();
	TargetYawRot.Pitch = 0.f;
	TargetYawRot.Roll = 0.f;

	const FRotator CurRot = GetActorRotation();
	const FRotator SmoothRot = FMath::RInterpTo(CurRot, TargetYawRot, DeltaTime, 15.f);

	SetActorRotation(SmoothRot);
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

	// SpringArm 회전
	FRotator DesiredRot;

	const bool bLockOn = ShouldLockOnTarget();
	
	if (bLockOn && Target )
	{
		const FVector From = SpringArm->GetComponentLocation(); // 또는 CameraPivot
		const FVector ToTarget = Target->GetActorLocation() - From;

		DesiredRot = ToTarget.Rotation();
		DesiredRot += TargetState->PivotRotation;

		// 강제 락온 -> 상대방을 향해 yaw회전 
		ApplyLockOnRotation(DeltaTime);
		/*{
			const FVector TargetVec = Target->GetActorLocation() - GetActorLocation();

			FRotator TargetYawRot = TargetVec.Rotation();
			TargetYawRot.Pitch = 0;
			TargetYawRot.Roll = 0;

			const FRotator CurtRot = GetActorRotation();
			const FRotator SmoothRot = FMath::RInterpTo(CurtRot, TargetYawRot, GetWorld()->GetDeltaSeconds(), 15.f);
			SetActorRotation(SmoothRot);
		}*/

		// Controller 회전값 동기화
		Controller->SetControlRotation(GetActorRotation());
	}
	else
	{
		// 공격 이외에는 자유 시점 
		DesiredRot = TargetState->PivotRotation + GetControlRotation();
	}

	SpringArm->SetWorldRotation(
		FMath::RInterpTo(
			SpringArm->GetComponentRotation(),
			DesiredRot,
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

bool APlayerBase::CanMove() const
{
	return !IsActionBlockedState() && IsMoveState();
}

void AClientPlayer::Dash()
{
	if (!CanDash())
		return;

	PlayDashMontage();

	// TODO: 대시 이동 처리
	// TODO: 서버에 대시 패킷 전송
}

void AClientPlayer::Dodge()
{
	if (!CanDodge())
		return;

	PlayDodgeMontage();

	// TODO: 회피 이동 처리
	// TODO: 서버에 회피 패킷 전송
}

void AClientPlayer::UseSkill(int32 SkillId)
{
	if (!CanUseSkill())
		return;

	if (GetCurPlayingSkill() != Protocol::SKILL_INFO_NONE)
		return;

	Protocol::C_SKILL SkillPkt;

	SkillPkt.set_skill_id(static_cast<Protocol::SkillId>(SkillId));

	Protocol::Vec3* Pos = SkillPkt.mutable_pos();
	const FVector Location = GetActorLocation();
	Pos->set_x(Location.X);
	Pos->set_y(Location.Y);
	Pos->set_z(Location.Z);

	SkillPkt.set_yaw(GetObjectInfo().yaw());

	SEND_PACKET_NO_SESSION(SkillPkt);
}

void AClientPlayer::Attack()
{
	if (!CanAttack())
		return;

	PlayAttackMontage();

	// TODO: 공격 판정 시작
	// TODO: 서버에 공격/스킬 패킷 전송
}

void AClientPlayer::TryParry()
{
	if (!CanUseSkill())
		return;

	PlayParryMontage();

	// TODO: 패링 가능 시간 활성화
	// TODO: 서버에 패링 요청 패킷 전송
}

void AClientPlayer::HandleDamage(const Protocol::S_DAMAGED& DamageInfo)
{
	if (!CanTakeDamage())
		return;
	
	PlayHitMontage();
	// TODO: DamageInfo에서 피해량 / 공격자 / 피격 방향 확인
	// TODO: 카메라 흔들림 / 피격 이펙트
	// TODO: 체력 UI 갱신
}

void AClientPlayer::Die()
{
	if (!CanDie())
		return;

	PlayDeathMontage();
	// TODO: 입력 비활성화
	// TODO: 카메라 연출
	// TODO: 리스폰 UI 표시
}

void AClientPlayer::EnterGroggy()
{
	PlayGroggyMontage();
	// TODO: 이동 / 공격 / 스킬 입력 제한
	// TODO: 그로기 이펙트 표시
}

void AClientPlayer::RecoverFromGroggy()
{
	if (!CanRecoverFromGroggy())
		return;

	PlayGroggyRecoverMontage();

	// TODO: 그로기 해제 애니메이션 또는 상태 복구
	// TODO: 이동 / 공격 / 스킬 입력 재활성화
	// TODO: 그로기 이펙트 제거
}

void AClientPlayer::PlayDashMontage()
{
	// TODO: Dash Montage 재생
}

void AClientPlayer::PlayDodgeMontage()
{
	// TODO: Dodge Montage 재생
}

void AClientPlayer::PlayAttackMontage()
{
	// TODO: Attack Montage 재생
}

void AClientPlayer::PlayParryMontage()
{
	// TODO: Parry Montage 재생
}

void AClientPlayer::PlayHitMontage()
{
	// TODO: Hit Montage 재생
}

void AClientPlayer::PlayDeathMontage()
{
	// TODO: Death Montage 재생
}

void AClientPlayer::PlayGroggyMontage()
{
	// TODO: Groggy Montage 재생
}

void AClientPlayer::PlayGroggyRecoverMontage()
{
	// TODO: Groggy Recover Montage 재생
}