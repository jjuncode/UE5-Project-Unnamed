#include "ServerPacketHandler.h"
#include "UE5_CppServer.h"

extern PacketHandlerFunc GPacketHandler[UINT16_MAX]{};

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return false;

	// TODO : 캐릭터 선택해서 idx전송
	Protocol::ObjectInfo playerInfo = pkt.player_info();

	// 입장 UI버튼 눌러서 게임 입장
	Protocol::C_ENTER_GAME enterGamePkt;

	static int32 PlayerId = 0;
	enterGamePkt.set_player_id(PlayerId++);	// 첫번째 캐릭터로 강제입장  -> fuck

	SEND_PACKET(session, enterGamePkt);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success() == false)
		return false;

	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		// TODO : 종료 로직
	}

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleSpawn(pkt);
	}
	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleDespawn(pkt);
	}
	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleMove(pkt);
	}
	return true;
}
