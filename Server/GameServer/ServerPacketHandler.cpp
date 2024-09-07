#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"
#include "Struct.pb.h"
#include "Room.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"
#include "Lobby.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_CTS_LOGIN(PacketSessionRef& session, Protocol::CTS_LOGIN& pkt)
{
	// TODO : DB에서 Account 정보 긁어온다
	// TODO : DB에서 유저 정보 긁어온다

	Protocol::STC_LOGIN loginPkt;
	if(pkt.id() == "Admin" && pkt.pw() == "Admin")
	{
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->set_player_id(pkt.player_id());
		playerInfo->set_allocated_player_name(new string("Admin"));
		playerInfo->set_player_type(Protocol::PlayerType::PLAYER_TYPE_AURORA);
		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		objectInfo->set_object_id(0);
		objectInfo->set_object_type(Protocol::ObjectType::OBJECT_TYPE_CREATURE);

		Protocol::PosInfo* posInfo = new Protocol::PosInfo();
		objectInfo->set_allocated_pos_info(posInfo);

		playerInfo->set_allocated_object_info(objectInfo);

		loginPkt.set_allocated_player(playerInfo);
		//loginPkt.set_id(pkt.id());
		loginPkt.set_success(true);
	}
	else
	{
		loginPkt.set_success(false);
	}

	

	SEND_PACKET(loginPkt);

	return true;
}

bool Handle_CTS_ENTER_LOBBY(PacketSessionRef& session, Protocol::CTS_ENTER_LOBBY& pkt)
{
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));

	player->SetPlayerInfo(pkt.player());

	if (player == nullptr)
		return false;

	GLobby->DoAsync(&Lobby::HandleEnterPlayer, player);

	return true;
}

bool Handle_CTS_CREATE_ROOM(PacketSessionRef& session, Protocol::CTS_CREATE_ROOM& pkt)
{
	GLobby->DoAsync(&Lobby::HandleCreateRoom, pkt.room_info());
	return true;
}

bool Handle_CTS_JOIN_ROOM(PacketSessionRef& session, Protocol::CTS_JOIN_ROOM& pkt)
{
	//GLobby->DoAsync(&Lobby::HandleJoinRoom, pkt);
	uint64 playerId = pkt.player().player_id();
	uint64 roomId = pkt.roomindex();
	GLobby->HandleJoinRoom(playerId, roomId);
	return true;
}

bool Handle_CTS_ENTER_GAME(PacketSessionRef& session, Protocol::CTS_ENTER_GAME& pkt)
{
	// 플레이어 생성
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));

	// 방에 입장
	GRoom->DoAsync(&Room::HandleEnterPlayer, player);

	return true;
}

bool Handle_CTS_LEAVE_ROOM(PacketSessionRef& session, Protocol::CTS_LEAVE_ROOM& pkt)
{
	return false;
}

bool Handle_CTS_LEAVE_GAME(PacketSessionRef& session, Protocol::CTS_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	//PlayerRef player = gameSession->player.load();
	//if (player == nullptr)
	//	return false;

	//RoomRef room = player->room.load().lock();
	//if (room == nullptr)
	//	return false;

	//room->HandleLeavePlayer(player);

	return true;
}

bool Handle_CTS_MOVE(PacketSessionRef& session, Protocol::CTS_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	//PlayerRef player = gameSession->player.load();
	//if (player == nullptr)
	//	return false;

	//RoomRef room = player->room.load().lock();
	//if (room == nullptr)
	//	return false;

	//room->DoAsync(&Room::HandleMove, pkt);
	//room->HandleMove(pkt);

	return true;
}

bool Handle_CTS_CHAT(PacketSessionRef& session, Protocol::CTS_CHAT& pkt)
{
	return false;
}
