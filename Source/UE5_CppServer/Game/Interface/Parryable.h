// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Protocol.pb.h"
#include "Parryable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UParryable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE5_CPPSERVER_API IParryable
{
	GENERATED_BODY()

public:
	virtual void Parry() = 0;	// 패링 수행 ( 캐싱해둔 정보로 수행 ) 
	virtual void CachingParryAttackInfo(Protocol::S_PARRY ParryInfo) = 0;	// 패링정보 캐싱
};
