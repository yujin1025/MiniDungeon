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
	PKT_CTS_EMAIL_VERIFICATION = 1000,
	PKT_STC_EMAIL_VERIFICATION = 1001,
	PKT_CTS_AUTH = 1002,
	PKT_STC_AUTH = 1003,
	PKT_CTS_CHECK_DUPLICATE = 1004,
	PKT_STC_CHECK_DUPLICATE = 1005,
	PKT_CTS_REGISTER = 1006,
	PKT_STC_REGISTER = 1007,
	PKT_CTS_LOGIN = 1008,
	PKT_STC_LOGIN = 1009,
	PKT_CTS_ENTER_LOBBY = 1010,
	PKT_STC_ENTER_LOBBY = 1011,
	PKT_CTS_CREATE_ROOM = 1012,
	PKT_STC_CREATE_ROOM = 1013,
	PKT_CTS_JOIN_ROOM = 1014,
	PKT_STC_JOIN_ROOM = 1015,
	PKT_CTS_CHANGE_CHARACTER = 1016,
	PKT_STC_CHANGE_CHARACTER = 1017,
	PKT_CTS_LEAVE_ROOM = 1018,
	PKT_STC_LEAVE_ROOM = 1019,
	PKT_CTS_ENTER_GAME = 1020,
	PKT_STC_ENTER_GAME = 1021,
	PKT_CTS_LEAVE_GAME = 1022,
	PKT_STC_LEAVE_GAME = 1023,
	PKT_CTS_SPAWN = 1024,
	PKT_STC_SPAWN = 1025,
	PKT_STC_DESPAWN = 1026,
	PKT_CTS_MOVE = 1027,
	PKT_STC_MOVE = 1028,
	PKT_CTS_DETECT = 1029,
	PKT_STC_DETECT = 1030,
	PKT_CTS_MONSTER_ATTACK = 1031,
	PKT_STC_MONSTER_ATTACK = 1032,
	PKT_CTS_ATTACKED = 1033,
	PKT_STC_ATTACKED = 1034,
	PKT_CTS_CHAT = 1035,
	PKT_STC_CHAT = 1036,
	PKT_CTS_ATTACK = 1037,
	PKT_STC_ATTACK = 1038,
	PKT_CTS_MONSTERINFO = 1039,
	PKT_STC_MONSTERINFO = 1040,
	PKT_CTS_MONSTERMOVE = 1041,
	PKT_STC_MONSTERMOVE = 1042,
	PKT_CTS_STANDARD_MONSTER = 1043,
	PKT_STC_STANDARD_MONSTER = 1044,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_CTS_EMAIL_VERIFICATION(PacketSessionRef& session, Protocol::CTS_EMAIL_VERIFICATION& pkt);
bool Handle_CTS_AUTH(PacketSessionRef& session, Protocol::CTS_AUTH& pkt);
bool Handle_CTS_CHECK_DUPLICATE(PacketSessionRef& session, Protocol::CTS_CHECK_DUPLICATE& pkt);
bool Handle_CTS_REGISTER(PacketSessionRef& session, Protocol::CTS_REGISTER& pkt);
bool Handle_CTS_LOGIN(PacketSessionRef& session, Protocol::CTS_LOGIN& pkt);
bool Handle_CTS_ENTER_LOBBY(PacketSessionRef& session, Protocol::CTS_ENTER_LOBBY& pkt);
bool Handle_CTS_CREATE_ROOM(PacketSessionRef& session, Protocol::CTS_CREATE_ROOM& pkt);
bool Handle_CTS_JOIN_ROOM(PacketSessionRef& session, Protocol::CTS_JOIN_ROOM& pkt);
bool Handle_CTS_CHANGE_CHARACTER(PacketSessionRef& session, Protocol::CTS_CHANGE_CHARACTER& pkt);
bool Handle_CTS_LEAVE_ROOM(PacketSessionRef& session, Protocol::CTS_LEAVE_ROOM& pkt);
bool Handle_CTS_ENTER_GAME(PacketSessionRef& session, Protocol::CTS_ENTER_GAME& pkt);
bool Handle_CTS_LEAVE_GAME(PacketSessionRef& session, Protocol::CTS_LEAVE_GAME& pkt);
bool Handle_CTS_SPAWN(PacketSessionRef& session, Protocol::CTS_SPAWN& pkt);
bool Handle_CTS_MOVE(PacketSessionRef& session, Protocol::CTS_MOVE& pkt);
bool Handle_CTS_DETECT(PacketSessionRef& session, Protocol::CTS_DETECT& pkt);
bool Handle_CTS_MONSTER_ATTACK(PacketSessionRef& session, Protocol::CTS_MONSTER_ATTACK& pkt);
bool Handle_CTS_ATTACKED(PacketSessionRef& session, Protocol::CTS_ATTACKED& pkt);
bool Handle_CTS_CHAT(PacketSessionRef& session, Protocol::CTS_CHAT& pkt);
bool Handle_CTS_ATTACK(PacketSessionRef& session, Protocol::CTS_ATTACK& pkt);
bool Handle_CTS_MONSTERINFO(PacketSessionRef& session, Protocol::CTS_MONSTERINFO& pkt);
bool Handle_CTS_MONSTERMOVE(PacketSessionRef& session, Protocol::CTS_MONSTERMOVE& pkt);
bool Handle_CTS_STANDARD_MONSTER(PacketSessionRef& session, Protocol::CTS_STANDARD_MONSTER& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_CTS_EMAIL_VERIFICATION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_EMAIL_VERIFICATION>(Handle_CTS_EMAIL_VERIFICATION, session, buffer, len); };
		GPacketHandler[PKT_CTS_AUTH] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_AUTH>(Handle_CTS_AUTH, session, buffer, len); };
		GPacketHandler[PKT_CTS_CHECK_DUPLICATE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_CHECK_DUPLICATE>(Handle_CTS_CHECK_DUPLICATE, session, buffer, len); };
		GPacketHandler[PKT_CTS_REGISTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_REGISTER>(Handle_CTS_REGISTER, session, buffer, len); };
		GPacketHandler[PKT_CTS_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_LOGIN>(Handle_CTS_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_CTS_ENTER_LOBBY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_ENTER_LOBBY>(Handle_CTS_ENTER_LOBBY, session, buffer, len); };
		GPacketHandler[PKT_CTS_CREATE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_CREATE_ROOM>(Handle_CTS_CREATE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_CTS_JOIN_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_JOIN_ROOM>(Handle_CTS_JOIN_ROOM, session, buffer, len); };
		GPacketHandler[PKT_CTS_CHANGE_CHARACTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_CHANGE_CHARACTER>(Handle_CTS_CHANGE_CHARACTER, session, buffer, len); };
		GPacketHandler[PKT_CTS_LEAVE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_LEAVE_ROOM>(Handle_CTS_LEAVE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_CTS_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_ENTER_GAME>(Handle_CTS_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_CTS_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_LEAVE_GAME>(Handle_CTS_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_CTS_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_SPAWN>(Handle_CTS_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_CTS_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_MOVE>(Handle_CTS_MOVE, session, buffer, len); };
		GPacketHandler[PKT_CTS_DETECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_DETECT>(Handle_CTS_DETECT, session, buffer, len); };
		GPacketHandler[PKT_CTS_MONSTER_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_MONSTER_ATTACK>(Handle_CTS_MONSTER_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_CTS_ATTACKED] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_ATTACKED>(Handle_CTS_ATTACKED, session, buffer, len); };
		GPacketHandler[PKT_CTS_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_CHAT>(Handle_CTS_CHAT, session, buffer, len); };
		GPacketHandler[PKT_CTS_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_ATTACK>(Handle_CTS_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_CTS_MONSTERINFO] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_MONSTERINFO>(Handle_CTS_MONSTERINFO, session, buffer, len); };
		GPacketHandler[PKT_CTS_MONSTERMOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_MONSTERMOVE>(Handle_CTS_MONSTERMOVE, session, buffer, len); };
		GPacketHandler[PKT_CTS_STANDARD_MONSTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CTS_STANDARD_MONSTER>(Handle_CTS_STANDARD_MONSTER, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::STC_EMAIL_VERIFICATION& pkt) { return MakeSendBuffer(pkt, PKT_STC_EMAIL_VERIFICATION); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_AUTH& pkt) { return MakeSendBuffer(pkt, PKT_STC_AUTH); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_CHECK_DUPLICATE& pkt) { return MakeSendBuffer(pkt, PKT_STC_CHECK_DUPLICATE); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_REGISTER& pkt) { return MakeSendBuffer(pkt, PKT_STC_REGISTER); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_STC_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_ENTER_LOBBY& pkt) { return MakeSendBuffer(pkt, PKT_STC_ENTER_LOBBY); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_CREATE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_STC_CREATE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_JOIN_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_STC_JOIN_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_CHANGE_CHARACTER& pkt) { return MakeSendBuffer(pkt, PKT_STC_CHANGE_CHARACTER); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_LEAVE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_STC_LEAVE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_STC_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_STC_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_STC_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_STC_DESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_STC_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_DETECT& pkt) { return MakeSendBuffer(pkt, PKT_STC_DETECT); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_MONSTER_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_STC_MONSTER_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_ATTACKED& pkt) { return MakeSendBuffer(pkt, PKT_STC_ATTACKED); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_STC_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_STC_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_MONSTERINFO& pkt) { return MakeSendBuffer(pkt, PKT_STC_MONSTERINFO); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_MONSTERMOVE& pkt) { return MakeSendBuffer(pkt, PKT_STC_MONSTERMOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::STC_STANDARD_MONSTER& pkt) { return MakeSendBuffer(pkt, PKT_STC_STANDARD_MONSTER); }

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