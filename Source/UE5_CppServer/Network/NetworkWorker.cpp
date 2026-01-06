// Fill out your copyright notice in the Description page of Project Settings.
#include "Network/NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session)
	: Socket{Socket}, SessionRef{Session}
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorker Thread"));
}

RecvWorker::~RecvWorker()
{
}

bool RecvWorker::Init()
{
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		TArray<uint8> Packet;

		if (RecvPacket(OUT Packet))
		{
			if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
			{
				Session->RecvPacketQueue.Enqueue(Packet);
			}
		}
	}
	return 0;
}

void RecvWorker::Exit()
{
}

void RecvWorker::Stop()
{
	Running.store(false);
}

void RecvWorker::Destroy()
{
	delete Thread;
}

void RecvWorker::WaitForThread()
{
	Thread->WaitForCompletion();
}

bool RecvWorker::RecvPacket(TArray<uint8>& OutPacket)
{
	// Parsing Packet Header
	const int32 HeaderSize = sizeof(PacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (RecvDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	// ID, Size √ﬂ√‚
	PacketHeader Header = *reinterpret_cast<PacketHeader*>(HeaderBuffer.GetData());

	OutPacket = HeaderBuffer;

	TArray<uint8> DataBuffer;
	const int32 DataSize = Header.size - HeaderSize;
	if (DataSize == 0)
		return true;

	OutPacket.AddZeroed(DataSize);

	if (RecvDesiredBytes(&OutPacket[HeaderSize], DataSize) == false)
		return false;

	return true;
}

bool RecvWorker::RecvDesiredBytes(uint8* Results, int32 Size)
{
	uint32 PendingDataSize;
		if ( Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
		return false;

	int32 Offset = 0;

	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0) 
			return false;

		Offset += NumRead;
		Size -= NumRead;
	}

	return true;
}

SendWorker::SendWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session)
	: Socket{ Socket }, SessionRef{ Session }
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorker Thread"));
}

SendWorker::~SendWorker()
{
}

bool SendWorker::Init()
{
	return true;
}

uint32 SendWorker::Run()
{
	while (Running)
	{
		SendBufferRef SendBuffer;

		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			if (Session->SendPacketQueue.Dequeue(OUT SendBuffer))
			{
				SendPacket(SendBuffer);
			}
		}
	}
	return 0;
}

void SendWorker::Exit()
{
}

bool SendWorker::SendPacket(SendBufferRef SendBuffer)
{
	if ( SendDesiredBytes(SendBuffer->Buffer(), SendBuffer->WriteSize()) == false)
		return false;
	return true;
}

void SendWorker::Stop()
{
	Running.store(false);
}

void SendWorker::Destroy()
{
	delete Thread;
}

void SendWorker::WaitForThread()
{
	Thread->WaitForCompletion();
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		if (Socket == nullptr) 
			return false;

		int32 ByteSent = 0;
		if (Socket->Send(Buffer, Size, OUT ByteSent) == false)
			return false;

		Size -= ByteSent;
		Buffer += ByteSent;
	}

	return true;

}
