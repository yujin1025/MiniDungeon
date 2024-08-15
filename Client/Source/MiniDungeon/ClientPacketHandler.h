#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "MiniDungeon.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// Utils Func
class UMDNetworkManager* GetWorldNetwork(const PacketSessionRef& Session);

enum : uint16
{
	PKT_CTS_LOGIN = 1000,
	PKT_STC_LOGIN = 1001,
	PKT_CTS_ENTER_LOBBY = 1002,
	PKT_STC_ENTER_LOBBY = 1003,
	PKT_CTS_JOIN_OR_CREATE_ROOM = 1004,
	PKT_STC_JOIN_OR_CREATE_ROOM = 1005,
	PKT_CTS_ENTER_GAME = 1006,
	PKT_STC_ENTER_GAME = 1007,
	PKT_CTS_LEAVE_GAME = 1008,
	PKT_STC_LEAVE_GAME = 1009,
	PKT_STC_SPAWN = 1010,
	PKT_STC_DESPAWN = 1011,
	PKT_CTS_MOVE = 1012,
	PKT_STC_MOVE = 1013,
	PKT_CTS_CHAT = 1014,
	PKT_STC_CHAT = 1015,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_STC_LOGIN(PacketSessionRef& session, Protocol::STC_LOGIN& pkt);
bool Handle_STC_ENTER_LOBBY(PacketSessionRef& session, Protocol::STC_ENTER_LOBBY& pkt);
bool Handle_STC_JOIN_OR_CREATE_ROOM(PacketSessionRef& session, Protocol::STC_JOIN_OR_CREATE_ROOM& pkt);
bool Handle_STC_ENTER_GAME(PacketSessionRef& session, Protocol::STC_ENTER_GAME& pkt);
bool Handle_STC_LEAVE_GAME(PacketSessionRef& session, Protocol::STC_LEAVE_GAME& pkt);
bool Handle_STC_SPAWN(PacketSessionRef& session, Protocol::STC_SPAWN& pkt);
bool Handle_STC_DESPAWN(PacketSessionRef& session, Protocol::STC_DESPAWN& pkt);
bool Handle_STC_MOVE(PacketSessionRef& session, Protocol::STC_MOVE& pkt);
bool Handle_STC_CHAT(PacketSessionRef& session, Protocol::STC_CHAT& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_STC_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_LOGIN>(Handle_STC_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_STC_ENTER_LOBBY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_ENTER_LOBBY>(Handle_STC_ENTER_LOBBY, session, buffer, len); };
		GPacketHandler[PKT_STC_JOIN_OR_CREATE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_JOIN_OR_CREATE_ROOM>(Handle_STC_JOIN_OR_CREATE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_STC_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_ENTER_GAME>(Handle_STC_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_STC_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_LEAVE_GAME>(Handle_STC_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_STC_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_SPAWN>(Handle_STC_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_STC_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_DESPAWN>(Handle_STC_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_STC_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_MOVE>(Handle_STC_MOVE, session, buffer, len); };
		GPacketHandler[PKT_STC_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::STC_CHAT>(Handle_STC_CHAT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::CTS_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_CTS_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_ENTER_LOBBY& pkt) { return MakeSendBuffer(pkt, PKT_CTS_ENTER_LOBBY); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_JOIN_OR_CREATE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_CTS_JOIN_OR_CREATE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CTS_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_CTS_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_CTS_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::CTS_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_CTS_CHAT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

public:
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