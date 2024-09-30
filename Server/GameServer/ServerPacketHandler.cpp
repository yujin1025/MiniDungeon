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
#include "DBConnectionPool.h"
#include "DBBind.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_CTS_LOGIN(PacketSessionRef& session, Protocol::CTS_LOGIN& pkt)
{
	DBConnection* dbConnection = GDBConnectionPool->Pop();
	if (dbConnection == nullptr)
	{
		return false;
	}

	//DB에서 Account 정보 긁어온다, DB에서 유저 정보 긁어온다
	DBBind<1, 3> dbBind(*dbConnection, L"SELECT player_id, ID, Password FROM MDDB.AccountInfo WHERE ID = ?");	

	//WCHAR id[100];
	wstring convertToWStringID = Utils::stringToWString(pkt.id());
	//wcscpy_s(id, convertToWStringID.c_str());
	dbBind.BindParam(0, convertToWStringID);

	int32 outIndex;
	WCHAR outID[100];
	WCHAR outPW[100];

	dbBind.BindColumn(0, OUT outIndex);
	dbBind.BindColumn(1, OUT outID);
	dbBind.BindColumn(2, OUT outPW);

	ASSERT_CRASH(dbBind.Execute());

	bool auth = false;

	while (dbBind.Fetch())
	{
		string convertedOutID = Utils::WCHARToString(outID);
		string convertedOutPW = Utils::WCHARToString(outPW);

		string id = pkt.id();
		id.push_back('\0');

		string pw = pkt.pw();
		pw.push_back('\0');

		if (convertedOutID == id && convertedOutPW == pw)
		{
			auth = true;
		}
	}

	Protocol::STC_LOGIN loginPkt;
	if(auth == true)
	{
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->set_player_id(pkt.player_id());

		playerInfo->set_player_name(pkt.id());

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
	uint64 playerIndex = pkt.player().player_id();
	uint64 roomIndex = pkt.roomindex();

	GLobby->DoAsync(&Lobby::HandleJoinRoom, playerIndex, roomIndex);
	return true;
}

bool Handle_CTS_LEAVE_ROOM(PacketSessionRef& session, Protocol::CTS_LEAVE_ROOM& pkt)
{
	// Lobby에 Room이 없다면 문제
	if (GLobby->GetRooms().find(pkt.roomindex()) == GLobby->GetRooms().end())
	{
		return false;
	}

	GLobby->GetRooms()[pkt.roomindex()]->DoAsync(&Room::HandleLeavePlayer, pkt.player_id());
	return true;
}

bool Handle_CTS_CHANGE_CHARACTER(PacketSessionRef& session, Protocol::CTS_CHANGE_CHARACTER& pkt)
{
	GLobby->GetRooms()[pkt.roomindex()]->DoAsync(&Room::HandleChangeCharacter, pkt.player_id(), pkt.character());
	return true;
}

bool Handle_CTS_ENTER_GAME(PacketSessionRef& session, Protocol::CTS_ENTER_GAME& pkt)
{
	// Lobby에 Room이 없다면 문제
	if (GLobby->GetRooms().find(pkt.room_id()) == GLobby->GetRooms().end())
	{
		return false;
	}

	GLobby->GetRooms()[pkt.room_id()]->DoAsync(&Room::HandleStartGame);

	return true;
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
