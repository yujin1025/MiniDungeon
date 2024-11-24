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
#include "AuthManager.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_CTS_EMAIL_VERIFICATION(PacketSessionRef& session, Protocol::CTS_EMAIL_VERIFICATION& pkt)
{
	Protocol::STC_EMAIL_VERIFICATION emailVerificationPkt;

	bool isExist = false;
	DBConnection* dbConnection = GDBConnectionPool->Pop();
	if (dbConnection == nullptr)
	{
		emailVerificationPkt.set_success(false);
	}
	else
	{
		DBBind<1, 1> dbBind(*dbConnection, L"SELECT e_mail FROM MDDB.AccountInfo WHERE e_mail = ?");

		wstring convertToWStringEmail = Utils::stringToWString(pkt.email());
		dbBind.BindParam(0, convertToWStringEmail);

		WCHAR outEmail[100];
		dbBind.BindColumn(0, OUT outEmail);

		ASSERT_CRASH(dbBind.Execute());
		GDBConnectionPool->Push(dbConnection);

		while (dbBind.Fetch())
		{
			string convertedOutID = Utils::WCHARToString(outEmail);

			string email = pkt.email();
			email.push_back('\0');

			if(convertedOutID == email)
			{
				isExist = true;
			}
		}

		if (isExist == true)
		{
			emailVerificationPkt.set_success(false);
		}
		else
		{
			AuthManager& authManager = AuthManager::GetInstance();

			if (authManager.AddAuthWaiter(pkt.email()) == false)
			{
				emailVerificationPkt.set_success(false);
			}
			else
			{
				emailVerificationPkt.set_success(true);
			}
		}
	}

	SEND_PACKET(emailVerificationPkt);

	return true;
}

bool Handle_CTS_AUTH(PacketSessionRef& session, Protocol::CTS_AUTH& pkt)
{
	Protocol::STC_AUTH authPkt;
	AuthManager& authManager = AuthManager::GetInstance();

	if(authManager.CheckAuthWaiter(pkt.email(), pkt.auth_code()) == false)
	{
		authPkt.set_success(false);
	}
	else
	{
		authPkt.set_success(true);
	}

	SEND_PACKET(authPkt);

	return true;
}

bool Handle_CTS_CHECK_DUPLICATE(PacketSessionRef& session, Protocol::CTS_CHECK_DUPLICATE& pkt)
{
	Protocol::STC_CHECK_DUPLICATE checkDuplicatePkt;
	bool isDuplicate = false;

	DBConnection* dbConnection = GDBConnectionPool->Pop();
	if (dbConnection == nullptr)
	{
		checkDuplicatePkt.set_is_duplicate(true);
	}
	else
	{
		DBBind<1, 1> dbBind(*dbConnection, L"SELECT ID FROM MDDB.AccountInfo WHERE ID = ?");

		wstring convertToWStringID = Utils::stringToWString(pkt.id());
		dbBind.BindParam(0, convertToWStringID);

		WCHAR outID[100];
		dbBind.BindColumn(0, OUT outID);

		ASSERT_CRASH(dbBind.Execute());
		GDBConnectionPool->Push(dbConnection);

		while (dbBind.Fetch())
		{
			string convertedOutID = Utils::WCHARToString(outID);
			if (convertedOutID == pkt.id())
			{
				isDuplicate = true;
			}
		}


		checkDuplicatePkt.set_is_duplicate(isDuplicate);
	}

	SEND_PACKET(checkDuplicatePkt);

	return false;
}

bool Handle_CTS_REGISTER(PacketSessionRef& session, Protocol::CTS_REGISTER& pkt)
{
	Protocol::STC_REGISTER registerPkt;

	DBConnection* dbConnection = GDBConnectionPool->Pop();
	if (dbConnection == nullptr)
	{
		registerPkt.set_success(false);
	}
	else
	{
		DBBind<3, 0> dbBind(*dbConnection, L"INSERT INTO MDDB.AccountInfo(ID, Password, e_mail) VALUES (?, ?, ?)");

		wstring convertToWStringID = Utils::stringToWString(pkt.id());
		dbBind.BindParam(0, convertToWStringID);

		wstring convertToWStringHashPW = Utils::sha256(pkt.pw());
		dbBind.BindParam(1, convertToWStringHashPW);

		wstring convertToWStringEmail = Utils::stringToWString(pkt.email());
		dbBind.BindParam(2, convertToWStringEmail);

		ASSERT_CRASH(dbBind.Execute());
		GDBConnectionPool->Push(dbConnection);

		registerPkt.set_success(true);
	}

	GDBConnectionPool->Push(dbConnection);

	SEND_PACKET(registerPkt);

	return true;
}

bool Handle_CTS_LOGIN(PacketSessionRef& session, Protocol::CTS_LOGIN& pkt)
{
	// DB 연결 풀에서 연결 가져오기
	DBConnection* dbConnection = GDBConnectionPool->Pop();
	if (dbConnection == nullptr)
	{
		return false;
	}

	// SQL 바인딩 및 실행
	DBBind<2, 1> dbBind(*dbConnection, L"SELECT player_id FROM MDDB.AccountInfo WHERE ID = ? AND Password = ? LIMIT 1");
	wstring wID = Utils::stringToWString(pkt.id());
	dbBind.BindParam(0, wID);
	string pw = pkt.pw();
	if(pw != "Admin" && pw != "Admin1" && pw != "0" && pw != "1")
	{
		wstring wPW = Utils::sha256(pkt.pw());
		dbBind.BindParam(1, wPW);
	}
	else
	{
		wstring wPW = Utils::stringToWString(pkt.pw());
		dbBind.BindParam(1, wPW);
	}

	int32 outIndex = 0;
	dbBind.BindColumn(0, OUT outIndex);

	if (!dbBind.Execute())
	{
		GDBConnectionPool->Push(dbConnection);
		return false;
	}

	// 인증 로직
	bool auth = false;

	while (dbBind.Fetch())
	{	
		// Fetch 성공 시, 결과에서 player_id를 읽어온 상태
		if (outIndex > 0) // player_id가 유효한 경우 인증 성공
		{
			auth = true;
			break; // 첫 번째 결과만 필요하므로 루프 종료
		}
	}

	GDBConnectionPool->Push(dbConnection);

	// 응답 패킷 작성
	Protocol::STC_LOGIN loginPkt;
	loginPkt.set_success(auth);

	if (auth)
	{
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->set_player_id(outIndex);

		playerInfo->set_player_name(pkt.id());

		playerInfo->set_player_type(Protocol::PlayerType::PLAYER_TYPE_AURORA);

		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		objectInfo->set_object_id(0);
		objectInfo->set_object_type(Protocol::ObjectType::OBJECT_TYPE_CREATURE);

		Protocol::PosInfo* posInfo = new Protocol::PosInfo();
		objectInfo->set_allocated_pos_info(posInfo);

		playerInfo->set_allocated_object_info(objectInfo);

		loginPkt.set_allocated_player(playerInfo);
		loginPkt.set_success(true);
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
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	GLobby->DoAsync(&Lobby::HandleCreateRoom, pkt.room_info());
	return true;
}

bool Handle_CTS_JOIN_ROOM(PacketSessionRef& session, Protocol::CTS_JOIN_ROOM& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	uint64 playerIndex = pkt.player().player_id();
	uint64 roomIndex = pkt.roomindex();

	GLobby->DoAsync(&Lobby::HandleJoinRoom, playerIndex, roomIndex);
	return true;
}

bool Handle_CTS_LEAVE_ROOM(PacketSessionRef& session, Protocol::CTS_LEAVE_ROOM& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	// Lobby에 Room이 없다면 문제
	if (GLobby->GetRooms().find(pkt.roomindex()) == GLobby->GetRooms().end())
	{
		return false;
	}

	GLobby->GetRooms()[pkt.roomindex()]->DoAsync(&Room::HandleLeavePlayer, pkt.player_id(), false);
	return true;
}

bool Handle_CTS_CHANGE_CHARACTER(PacketSessionRef& session, Protocol::CTS_CHANGE_CHARACTER& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	GLobby->GetRooms()[pkt.roomindex()]->DoAsync(&Room::HandleChangeCharacter, pkt.player_id(), pkt.character());
	return true;
}

bool Handle_CTS_ENTER_GAME(PacketSessionRef& session, Protocol::CTS_ENTER_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	// Lobby에 Room이 없다면 문제
	if (GLobby->GetRooms().find(pkt.room_id()) == GLobby->GetRooms().end())
	{
		return false;
	}

	GLobby->GetRooms()[pkt.room_id()]->DoAsync(&Room::HandleStartGame);

	return true;
}

bool Handle_CTS_SPAWN(PacketSessionRef& session, Protocol::CTS_SPAWN& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	if (GLobby->GetRooms().find(pkt.room_id()) == GLobby->GetRooms().end())
	{
		return false;
	}

	GLobby->GetRooms()[pkt.room_id()]->DoAsync(&Room::Spawn, pkt.creature_type(), pkt.pos_info());

	return false;
}


bool Handle_CTS_LEAVE_GAME(PacketSessionRef& session, Protocol::CTS_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleLeavePlayer, player->GetPlayerInfo().player_id(), true);

	return true;
}

bool Handle_CTS_MOVE(PacketSessionRef& session, Protocol::CTS_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleMove, pkt.info());

	return true;
}

bool Handle_CTS_DETECT(PacketSessionRef& session, Protocol::CTS_DETECT& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;
	
	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	return true;
}

bool Handle_CTS_MONSTER_ATTACK(PacketSessionRef& session, Protocol::CTS_MONSTER_ATTACK& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	Protocol::STC_MONSTER_ATTACK monsterAttackPkt;
	monsterAttackPkt.set_monster_id(pkt.monster_id());

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(monsterAttackPkt);
	room->Broadcast(sendBuffer);

	return true;
}

bool Handle_CTS_ATTACKED(PacketSessionRef& session, Protocol::CTS_ATTACKED& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleAttacked, player->GetObjectInfo().object_id(), pkt);

	return true;
}

bool Handle_CTS_ATTACK(PacketSessionRef& session, Protocol::CTS_ATTACK& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	//room->DoAsync(&Room::HandleAttack, pkt.info());
	room->DoAsync(&Room::HandleAttack, pkt);
	//room->HandleAttack(pkt);

	return true;
}

bool Handle_CTS_MONSTERINFO(PacketSessionRef& session, Protocol::CTS_MONSTERINFO& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	// 서버에서 몬스터 소환
	//room->DoAsync(&Room::SpawnMonster);

	return true;
}

bool Handle_CTS_MONSTERMOVE(PacketSessionRef& session, Protocol::CTS_MONSTERMOVE& pkt)
{
	return true;
}

bool Handle_CTS_STANDARD_MONSTER(PacketSessionRef& session, Protocol::CTS_STANDARD_MONSTER& pkt)
{
	return true;
}

bool Handle_CTS_CHAT(PacketSessionRef& session, Protocol::CTS_CHAT& pkt)
{
	return true;
}
