#include "ServerPacketHandler.h"
#include "UE5_CppServer.h"
#include "atomic"

extern PacketHandlerFunc GPacketHandler[UINT16_MAX]{};
static std::atomic<int32> PlayerId = 0;

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

	enterGamePkt.set_player_id(PlayerId.fetch_add(1));	// 임시 ID로 입장중 -> 서버에서 발급해줄거임

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
		PlayerId.store(0);
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

bool Handle_S_SPAWNDUMMY(PacketSessionRef& session, Protocol::S_SPAWNDUMMY& pkt)
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

bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleSkill(pkt);
	}
	return true;
}

bool Handle_S_DAMAGED(PacketSessionRef& session, Protocol::S_DAMAGED& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleDamaged(pkt);
	}
	return true;
}

bool Handle_S_PARRY(PacketSessionRef& session, Protocol::S_PARRY& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleParry(pkt);
	}
	return true;
}

bool Handle_S_SELECT_ENEMY(PacketSessionRef& session, Protocol::S_SELECT_ENEMY& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleSelectEnemy(pkt);
	}
	return true;
}

bool Handle_S_DEBUG(PacketSessionRef& session, Protocol::S_DEBUG& pkt)
{
	if (auto* GameManager = GetManager<UGameManager>())
	{
		GameManager->HandleDebugMessage(pkt);
	}
	return true;
}
