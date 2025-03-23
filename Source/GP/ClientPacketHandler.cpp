#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "GPGameInstance.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_SC_ENTER_GAME(SessionRef& session, Protocol::SC_ENTER_GAME& pkt)
{
	if (pkt.success() == false) return false;

	if (auto* GameInstance = Cast<UGPGameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_SC_LEAVE_GAME(SessionRef& session, Protocol::SC_LEAVE_GAME& pkt)
{
	return true;
}

bool Handle_SC_SPAWN(SessionRef& session, Protocol::SC_SPAWN& pkt)
{
	auto* GameInstance = Cast<UGPGameInstance>(GWorld->GetGameInstance());
	if (GameInstance == nullptr) return false;

	for (auto& player : pkt.players())
	{
		GameInstance->HandleSpawn(player);
	}
	return true;
}

bool Handle_SC_DESPAWN(SessionRef& session, Protocol::SC_DESPAWN& pkt)
{
	return true;
}

bool Handle_SC_CHAT(SessionRef& session, Protocol::SC_CHAT& pkt)
{
	auto s = pkt.msg();

	return true;
}
