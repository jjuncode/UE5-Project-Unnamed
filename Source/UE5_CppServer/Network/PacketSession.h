// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetUtils.h"


class UE5_CPPSERVER_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(class FSocket* Socket);
	~PacketSession();

	void Run();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	void StopThread();
	void WaitForThread();
	void DestroyThread();

public:
	class FSocket* Socket;
	
	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;

	TQueue<TArray<uint8>> RecvPacketQueue;	// main <-> other threads Queue
	TQueue<SendBufferRef> SendPacketQueue;	// main <-> other threads Queue
};
