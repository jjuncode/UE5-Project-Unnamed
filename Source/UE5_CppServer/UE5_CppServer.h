// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PacketSession.h"
#include "Protocol.pb.h"
#include "Network/NetUtils.h"
#include "ServerPacketHandler.h"

#include "GameManager.h"
#include "NetworkManager.h"

template<typename T>
T* GetManager()
{
	if (auto* GameInstance = GWorld->GetGameInstance())
	{
		return GameInstance->GetSubsystem<T>();
	}

	return nullptr;
}

UFUNCTION(BlueprintCallable, Category = "Manager")
inline UNetworkManager* GetNetworkManager()
{
	return GetManager<UNetworkManager>();
}