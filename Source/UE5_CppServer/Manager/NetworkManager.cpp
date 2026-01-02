// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/NetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"

void UNetworkManager::ConnectServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("STREAM"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Addr->SetIp(Ip.Value);
	Addr->SetPort(Port);

	bool Connected = Socket->Connect(*Addr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));
		ClientSession = MakeShared<PacketSession>(Socket);
		ClientSession->Run();

		// TODO : Lobby에서 캐릭터 선택창 등 
		{
			Protocol::C_LOGIN Pkt;
			SendBufferRef SendBuffer = ServerPacketHandler::MakePKTSendBuffer(Pkt);
			SendPacket(SendBuffer);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UNetworkManager::HandleRecvPackets()
{
	if (Socket == nullptr || ClientSession == nullptr)
		return;

	ClientSession->HandleRecvPackets();
}

void UNetworkManager::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || ClientSession == nullptr)
		return;

	ClientSession->SendPacket(SendBuffer);
}

void UNetworkManager::DisconnectServer()
{
	if (Socket == nullptr || ClientSession == nullptr)
		return;

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(ClientSession, LeavePkt);
}

void UNetworkManager::Deinitialize()
{
	Super::Deinitialize();

	DisconnectServer();

	if (ClientSession != nullptr)
	{
		// Client Only ( not connected to server ) 
		ClientSession->StopThread();
		ClientSession->WaitForThread();
		ClientSession->DestroyThread();
	}
	ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get();
	SocketSubSystem->DestroySocket(Socket);
	Socket = nullptr;
}
