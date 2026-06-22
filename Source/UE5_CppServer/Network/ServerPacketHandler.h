#pragma once
#include "Protocol.pb.h"

#if WITH_ENGINE
#include "NetUtils.h"
#endif 

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[UINT16_MAX];

enum :uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_LEAVE_GAME = 1004,
	PKT_S_LEAVE_GAME = 1005,
	PKT_S_SPAWN = 1006,
	PKT_S_SPAWNDUMMY = 1007,
	PKT_S_DESPAWN = 1008,
	PKT_C_MOVE = 1009,
	PKT_S_MOVE = 1010,
	PKT_C_SKILL = 1011,
	PKT_S_SKILL = 1012,
	PKT_S_DAMAGED = 1013,
	PKT_S_PARRY = 1014,
	PKT_C_SELECT_ENEMY = 1015,
	PKT_S_SELECT_ENEMY = 1016,
	PKT_S_DEBUG = 1017,
};

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN&pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME&pkt);
bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME&pkt);
bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN&pkt);
bool Handle_S_SPAWNDUMMY(PacketSessionRef& session, Protocol::S_SPAWNDUMMY&pkt);
bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN&pkt);
bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE&pkt);
bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL&pkt);
bool Handle_S_DAMAGED(PacketSessionRef& session, Protocol::S_DAMAGED&pkt);
bool Handle_S_PARRY(PacketSessionRef& session, Protocol::S_PARRY&pkt);
bool Handle_S_SELECT_ENEMY(PacketSessionRef& session, Protocol::S_SELECT_ENEMY&pkt);
bool Handle_S_DEBUG(PacketSessionRef& session, Protocol::S_DEBUG&pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;
		g_packet_handler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN >(Handle_S_LOGIN, session, buffer, len); };
		g_packet_handler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME >(Handle_S_ENTER_GAME, session, buffer, len); };
		g_packet_handler[PKT_S_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEAVE_GAME >(Handle_S_LEAVE_GAME, session, buffer, len); };
		g_packet_handler[PKT_S_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SPAWN >(Handle_S_SPAWN, session, buffer, len); };
		g_packet_handler[PKT_S_SPAWNDUMMY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SPAWNDUMMY >(Handle_S_SPAWNDUMMY, session, buffer, len); };
		g_packet_handler[PKT_S_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DESPAWN >(Handle_S_DESPAWN, session, buffer, len); };
		g_packet_handler[PKT_S_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MOVE >(Handle_S_MOVE, session, buffer, len); };
		g_packet_handler[PKT_S_SKILL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SKILL >(Handle_S_SKILL, session, buffer, len); };
		g_packet_handler[PKT_S_DAMAGED] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DAMAGED >(Handle_S_DAMAGED, session, buffer, len); };
		g_packet_handler[PKT_S_PARRY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PARRY >(Handle_S_PARRY, session, buffer, len); };
		g_packet_handler[PKT_S_SELECT_ENEMY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SELECT_ENEMY >(Handle_S_SELECT_ENEMY, session, buffer, len); };
		g_packet_handler[PKT_S_DEBUG] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DEBUG >(Handle_S_DEBUG, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return g_packet_handler[header->id](session, buffer, len);
	}
	static SendBufferRef MakePKTSendBuffer(Protocol::C_LOGIN&pkt) { return _MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakePKTSendBuffer(Protocol::C_ENTER_GAME&pkt) { return _MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakePKTSendBuffer(Protocol::C_LEAVE_GAME&pkt) { return _MakeSendBuffer(pkt, PKT_C_LEAVE_GAME); }
	static SendBufferRef MakePKTSendBuffer(Protocol::C_MOVE&pkt) { return _MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakePKTSendBuffer(Protocol::C_SKILL&pkt) { return _MakeSendBuffer(pkt, PKT_C_SKILL); }
	static SendBufferRef MakePKTSendBuffer(Protocol::C_SELECT_ENEMY&pkt) { return _MakeSendBuffer(pkt, PKT_C_SELECT_ENEMY); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 data_size = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packet_size = data_size + sizeof(PacketHeader);

#if WITH_ENGINE
		SendBufferRef send_buffer = MakeShared<SendBuffer>(packet_size);
#else
		SendBufferRef send_buffer = g_send_buffer_mgr->Open(packet_size);
#endif
		PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->Buffer());
		header->size = packet_size;
		header->id = pktId;
#if WITH_ENGINE
		check(pkt.SerializeToArray(&header[1], data_size));
#else
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], data_size));
#endif

		send_buffer->Close(packet_size);

		return send_buffer;
	}
};