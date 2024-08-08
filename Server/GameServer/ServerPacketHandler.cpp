#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"
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
	// TODO : DB���� Account ���� �ܾ�´�
	// TODO : DB���� ���� ���� �ܾ�´�

	Protocol::STC_LOGIN loginPkt;
	if(pkt.id() == "Admin" && pkt.pw() == "Admin")
	{
		//Protocol::ObjectInfo* player = loginPkt.add_players();
		//Protocol::PosInfo* posInfo = player->mutable_pos_info();
		loginPkt.set_id(pkt.id());
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
	PlayerRef player = static_pointer_cast<GameSession>(session)->player;
	if (player == nullptr)
		return false;

	GLobby->DoAsync(&Lobby::HandleEnterPlayer, player);

	return true;
}

bool Handle_CTS_ENTER_GAME(PacketSessionRef& session, Protocol::CTS_ENTER_GAME& pkt)
{
	// �÷��̾� ����
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));

	// �濡 ����
	GRoom->DoAsync(&Room::HandleEnterPlayer, player);
	//GRoom->HandleEnterPlayerLocked(player);

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
