// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weaponable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE5_CPPSERVER_API IWeaponable
{
	GENERATED_BODY()

public:
	virtual void AttachWeapon(TSubclassOf<class AWeapon> Weapon) = 0;
};
