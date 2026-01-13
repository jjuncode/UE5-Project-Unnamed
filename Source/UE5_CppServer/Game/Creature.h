// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Protocol.pb.h"
#include "GameplayTagsClasses.h"
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
	Protocol::ObjectInfo& SetObjectInfoRef() { return ObjectInfo; }
	const Protocol::ObjectInfo& GetObjectInfo() const { return ObjectInfo; }

	void SetMoveState(const Protocol::MoveState& rhs);
	Protocol::MoveState GetMoveState() const { return ObjectInfo.creature_info().move_state(); }

	void SetActionState(const Protocol::ActionState& rhs);
	Protocol::ActionState GetActionState() const { return ObjectInfo.creature_info().action_state(); }

	// ----------------------
	//		Event Handle
	// ----------------------
	virtual void HandleEvent(FGameplayTag EventTag);

	// -------------------
	//		MoveSync
	// -------------------
	void SetDesntInfo(const Protocol::ObjectInfo& rhs){ DestnInfo.CopyFrom(rhs); }
	void SetMoveDir(const FVector& Vec);
	void SetMoveDir(const Protocol::Vec3& Dir) { MoveDir.CopyFrom(Dir); }

protected:
	Protocol::ObjectInfo ObjectInfo;

	// ----------------
	//		Cache
	// ----------------
	// Controller 
	TObjectPtr< class AController> Controller;

	// MoveSync
	Protocol::ObjectInfo DestnInfo;
	Protocol::Vec3 MoveDir;
};
