// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Protocol.pb.h"
#include "Creature.generated.h"

UCLASS()
class UE5_CPPSERVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	ACreature();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void SetObjectInfo(const Protocol::ObjectInfo& rhs) { ObjectInfo.CopyFrom(rhs); }
	const Protocol::ObjectInfo& GetObjectInfo() { return ObjectInfo; }

	void SetMoveState(const Protocol::MoveState& rhs);
	Protocol::MoveState GetMoveState() { return ObjectInfo.creature_info().state(); }

	void SetDesntInfo(const Protocol::ObjectInfo& rhs){ DestnInfo.CopyFrom(rhs); }
	void SetMoveDir(const FVector& Vec);
	void SetMoveDir(const Protocol::Vec3& Dir) { MoveDir.CopyFrom(Dir); }

protected:
	Protocol::ObjectInfo ObjectInfo;
	
	// MoveSync
	Protocol::ObjectInfo DestnInfo;
	Protocol::Vec3 MoveDir;
};
