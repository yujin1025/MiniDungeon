#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"

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

	for (int32 i = 0; i < 3; i++)
	{
		/*Protocol::ObjectInfo* player = loginPkt.add_players();
		Protocol::PosInfo* posInfo = player->mutable_pos_info();
		posInfo->set_x(Utils::GetRandom(0.f, 100.f));
		posInfo->set_y(Utils::GetRandom(0.f, 100.f));
		posInfo->set_z(Utils::GetRandom(0.f, 100.f));
		posInfo->set_yaw(Utils::GetRandom(0.f, 45.f));*/
	}

	loginPkt.set_success(true);

	SEND_PACKET(loginPkt);

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
