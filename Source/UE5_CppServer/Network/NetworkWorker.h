// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetUtils.h"

class FSocket;

class UE5_CPPSERVER_API RecvWorker : public FRunnable
{
public:
	RecvWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session);
	~RecvWorker();

	virtual bool	Init()	override;
	virtual uint32	Run()	override;
	virtual void	Exit()	override;

	void Stop();
	void Destroy();
	void WaitForThread();

private:
	bool RecvPacket(TArray<uint8>& OutPacket);
	bool RecvDesiredBytes(uint8* Results, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	std::atomic<bool> Running = true;
	FSocket* Socket;

	TWeakPtr<class PacketSession> SessionRef;
};

class UE5_CPPSERVER_API SendWorker : public FRunnable
{
public:
	SendWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session);
	~SendWorker();

	virtual bool	Init()	override;
	virtual uint32	Run()	override;
	virtual void	Exit()	override;

	bool SendPacket(SendBufferRef SendBuffer);

	void Stop();
	void Destroy();
	void WaitForThread();

private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	std::atomic<bool> Running = true;
	FSocket* Socket;

	TWeakPtr<class PacketSession> SessionRef;
};
