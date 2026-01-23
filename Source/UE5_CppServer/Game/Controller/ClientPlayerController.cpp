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
		
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientPlayerController::SyncYaw);
		
		// Mouse Look
		EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AClientPlayerController::HandleMouseLookAction);
		EIC->BindAction(MouseLookAction, ETriggerEvent::Completed, this, &AClientPlayerController::HandleMouseLookAction);
	
		// Skill 
		EIC->BindAction(SkillAction, ETriggerEvent::Started, this, &AClientPlayerController::HandleSkillAction);
	
		// Search Enemy
		EIC->BindAction(SearchEnemyAction, ETriggerEvent::Started, this, &AClientPlayerController::HandleSearchEnemyAction);
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
	// IDLE / RUN / BATTLE 상태에서만 움직이기가 가능하다
	Protocol::ActionState ActionState = ClientPlayer->GetActionState();

	if (ActionState == Protocol::ACTION_STATE_MOVE_IDLE
		|| ActionState == Protocol::ACTION_STATE_MOVE_RUN
		|| ActionState == Protocol::ACTION_STATE_BATTLE)
	{}
	else return;

	if (ActionState == Protocol::ACTION_STATE_BATTLE)
	{
		_HandleMoveAction(Value, Protocol::ACTION_STATE_BATTLE);
	}
	else
	{
		_HandleMoveAction(Value, Protocol::ACTION_STATE_MOVE_RUN);
	}
}

void AClientPlayerController::HandleMoveActionCompleted(const FInputActionValue& Value)
{
	ClientPlayer->ForceSendMovePkt();

	Protocol::ActionState ActionState = ClientPlayer->GetActionState();
	if (ActionState == Protocol::ACTION_STATE_BATTLE)
	{
		_HandleMoveAction(Value, Protocol::ACTION_STATE_BATTLE);
	}
	else
	{
		_HandleMoveAction(Value, Protocol::ACTION_STATE_MOVE_IDLE);
	}
}

void AClientPlayerController::SyncYaw(const FInputActionValue& Value)
{
	// 키 입력시 yaw 동기화 
	Protocol::ActionState ActionState = ClientPlayer->GetActionState();
	if (ActionState == Protocol::ACTION_STATE_MOVE_IDLE
		|| ActionState == Protocol::ACTION_STATE_MOVE_RUN
		|| ActionState == Protocol::ACTION_STATE_BATTLE ) 
	{
		FVector2D InputValue = Value.Get<FVector2D>();

		const FRotator ControlRot = GetControlRotation();
		const FRotator TargetYawRot(0.f, ControlRot.Yaw, 0.f);

		const FRotator CurrentRot = ClientPlayer->GetActorRotation();
		const float InterpSpeed = 15.f; // 값 키울수록 빠르게 회전
		const FRotator SmoothRot =
			FMath::RInterpTo(CurrentRot, TargetYawRot, GetWorld()->GetDeltaSeconds(), InterpSpeed);

		// 보간 회전
		ClientPlayer->SetActorRotation(SmoothRot);
	}
}

void AClientPlayerController::HandleMouseLookAction(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	Protocol::ActionState ActionState = ClientPlayer->GetActionState();

	// IDLE / MOVE / BATTLE 일 때만 카메라 회전이 가능 
	if (ActionState == Protocol::ACTION_STATE_MOVE_IDLE
		|| ActionState == Protocol::ACTION_STATE_MOVE_RUN
		|| ActionState == Protocol::ACTION_STATE_BATTLE )
	{}
	else return;

	AddYawInput(InputValue.X);
	AddPitchInput(-InputValue.Y);
}

void AClientPlayerController::HandleSkillAction(const FInputActionValue& Value)
{
	Protocol::ActionState ActionState = ClientPlayer->GetActionState();
	
	if (ActionState == Protocol::ACTION_STATE_MOVE_IDLE
		|| ActionState == Protocol::ACTION_STATE_MOVE_RUN
		|| ActionState == Protocol::ACTION_STATE_BATTLE )
	{}
	else
		return;

	if (ClientPlayer->GetCurPlayingSkill() != Protocol::SKILL_INFO_NONE)
		return;

	const int SkillIndex = Value.Get<float>();

	Protocol::C_SKILL SkillPkt;

	// 스킬 idx 설정 
	Protocol::SkillId SkillInfo = static_cast<Protocol::SkillId>(SkillIndex);
	SkillPkt.set_skill_id(SkillInfo);

	// 위치, yaw설정 
	Protocol::Vec3* Pos = SkillPkt.mutable_pos();
	FVector Location = ClientPlayer->GetActorLocation();
	Pos->set_x(Location.X);
	Pos->set_y(Location.Y);
	Pos->set_z(Location.Z);
	SkillPkt.set_yaw(ClientPlayer->GetObjectInfo().yaw());

	SEND_PACKET_NO_SESSION(SkillPkt);
}

void AClientPlayerController::HandleSearchEnemyAction(const FInputActionValue& Value)
{
	// 거리를 기준으로 먼저 주변의 creature들을 탐색 / 방향까지 탐색
	// target을 선별해서 보내줌
	auto* GameManager = GetGameManager();
	if (!GameManager || !ClientPlayer) return;

	if (ClientPlayer->GetTarget() != nullptr)
	{
		ClientPlayer->ResetTarget();
		ClientPlayer->SetCameraState(ECameraState::Normal);
		return;
	}

	const FVector MyPos = ClientPlayer->GetActorLocation();
	const FVector Forward = ClientPlayer->GetCameraForward(); // 카메라가 보는 기준 

	const float MaxDistSq = SearchEnemyDistn * SearchEnemyDistn;
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(SearchEnemyAngle));
	const float DotEpsilon = 0.01f; // 거의 정면 판별 기준

	APlayerBase* BestTarget = nullptr;
	float BestDot = MinDot;
	float BestDistSq = TNumericLimits<float>::Max();

	for (const auto& p : GameManager->GetPlayers())
	{
		APlayerBase* player = p.Value.Get();
		if (!player) continue;

		// 자신 제외
		if (GameManager->IsMyPlayer(player))
			continue;

		const FVector ToTarget = player->GetActorLocation() - MyPos;
		const float DistSq = ToTarget.SizeSquared();

		// 거리 컷
		if (DistSq > MaxDistSq)
			continue;

		// 각도 체크
		const FVector ToTargetDir = ToTarget * FMath::InvSqrt(DistSq);
		const float Dot = FVector::DotProduct(Forward, ToTargetDir);
		if (Dot < MinDot)
			continue;

		// ===== 최종 선택 로직 =====
		const bool bBetterDot = Dot > BestDot + DotEpsilon;
		const bool bSameDotButCloser =
			FMath::Abs(Dot - BestDot) <= DotEpsilon && DistSq < BestDistSq;

		if (bBetterDot || bSameDotButCloser)
		{
			BestDot = Dot;
			BestDistSq = DistSq;
			BestTarget = player;
		}
	}

	// ===== 결과 로그 =====
	if (BestTarget)
	{
		Protocol::C_SELECT_ENEMY SelectEnemyPkt;
		SelectEnemyPkt.set_target_id(BestTarget->GetObjectInfo().creature_info().id());
		SEND_PACKET_NO_SESSION(SelectEnemyPkt);

		const float BestDist = FMath::Sqrt(BestDistSq);
		const float AngleDeg =
			FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(BestDot, -1.f, 1.f)));

		const FString DebugMsg = FString::Printf(
			TEXT("[LockOn] Target=%s | Dist=%.1f | Dot=%.3f | Angle=%.1f"),
			*BestTarget->GetName(),
			BestDist,
			BestDot,
			AngleDeg
		);

		// Key = -1 → 매번 새 메시지
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,              // 표시 시간 (초)
			FColor::Green,
			DebugMsg
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Red,
			TEXT("[LockOn] No valid target")
		);
	}

	// Draw Debug Span 
	{
		const FVector Start = MyPos;

		// 중앙선 (정면)
		DrawDebugLine(
			GetWorld(),
			Start,
			Start + Forward * SearchEnemyDistn,
			FColor::Yellow,
			false,
			1.f,
			0,
			2.f
		);

		// 좌 / 우 경계 각도
		const FRotator LeftRot(0.f, -SearchEnemyAngle, 0.f);
		const FRotator RightRot(0.f, SearchEnemyAngle, 0.f);

		const FVector LeftDir = LeftRot.RotateVector(Forward);
		const FVector RightDir = RightRot.RotateVector(Forward);

		// 왼쪽 경계
		DrawDebugLine(
			GetWorld(),
			Start,
			Start + LeftDir * SearchEnemyDistn,
			FColor::Cyan,
			false,
			1.f,
			0,
			1.5f
		);

		// 오른쪽 경계
		DrawDebugLine(
			GetWorld(),
			Start,
			Start + RightDir * SearchEnemyDistn,
			FColor::Cyan,
			false,
			1.f,
			0,
			1.5f
		);
	}
}
