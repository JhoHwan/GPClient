#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
using SessionRef = TSharedPtr<class PacketSession>;
using SendBufferRef = TSharedPtr<class SendBuffer>;
#include "GP.h"
#else
using SessionRef = shared_ptr<Session>;
using SendBufferRef = shared_ptr<SendBuffer>;
#endif

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_CS_ENTER_GAME = 1000,
	PKT_SC_ENTER_GAME = 1001,
	PKT_CS_LEAVE_GAME = 1002,
	PKT_SC_LEAVE_GAME = 1003,
	PKT_SC_SPAWN = 1004,
	PKT_SC_DESPAWN = 1005,
	PKT_CS_CHAT = 1006,
	PKT_SC_CHAT = 1007,
	PKT_CS_REQUEST_MOVE = 1008,
};

// Custom Handlers
bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_SC_ENTER_GAME(SessionRef& session, Protocol::SC_ENTER_GAME& pkt);
bool Handle_SC_LEAVE_GAME(SessionRef& session, Protocol::SC_LEAVE_GAME& pkt);
bool Handle_SC_SPAWN(SessionRef& session, Protocol::SC_SPAWN& pkt);
bool Handle_SC_DESPAWN(SessionRef& session, Protocol::SC_DESPAWN& pkt);
bool Handle_SC_CHAT(SessionRef& session, Protocol::SC_CHAT& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_SC_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_ENTER_GAME>(Handle_SC_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_SC_LEAVE_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_LEAVE_GAME>(Handle_SC_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_SC_SPAWN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_SPAWN>(Handle_SC_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_SC_DESPAWN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_DESPAWN>(Handle_SC_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_SC_CHAT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_CHAT>(Handle_SC_CHAT, session, buffer, len); };
	}

	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::CS_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CS_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CS_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CS_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CS_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_CS_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::CS_REQUEST_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_CS_REQUEST_MOVE); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
#endif
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};