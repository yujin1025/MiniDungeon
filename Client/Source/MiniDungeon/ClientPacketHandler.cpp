#include "ClientPacketHandler.h"
//#include "ClientPacketHandler.h"
//#include "BufferReader.h"
//#include "S1.h"
//#include "S1GameInstance.h"
//#include "S1NetworkManager.h"
//
//PacketHandlerFunc GPacketHandler[UINT16_MAX];
//
///*
// * ���� ������ InstanceSubSystem�� US1NetworkManager�� �������� �Լ� �Դϴ�.
// * PIE�� ��� GWorld�� ������ ��� ù ��°�� �����ϴ� ������ �����ؼ� �ذ�����Դϴ�.
// */
//US1NetworkManager* GetWorldNetwork(const PacketSessionRef& Session)
//{
//	// ������ �����ϴ� ��� ���带 ��ȸ�ؼ� �ش���忡 SubGameInstance�� Session�� ���� ���մϴ�.
//	// SubGameInstance�� ��� PIE���� ���������� �����Ǳ� ������ Session�� �翬�� �и� �˴ϴ�.
//	for (auto World : GEngine->GetWorldContexts())
//	{
//		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
//		{
//			if (US1NetworkManager* NetworkManager = GameInstance->GetSubsystem<US1NetworkManager>())
//			{
//				if (NetworkManager->GameServerSession == Session)
//					return NetworkManager;
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
//{
//	return false;
//}
//
//bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
//{
//	for (auto& Player : pkt.players())
//	{
//	}
//
//	for (int32 i = 0; i < pkt.players_size(); i++)
//	{
//		const Protocol::ObjectInfo& Player = pkt.players(i);
//	}
//
//	// �κ񿡼� ĳ���� �����ؼ� �ε��� ����.
//	Protocol::C_ENTER_GAME EnterGamePkt;
//	EnterGamePkt.set_playerindex(0);
//
//	if (const US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		GameNetwork->SendPacket(EnterGamePkt);
//	}
//
//	return true;
//}
//
//bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
//{
//	if (US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		GameNetwork->HandleSpawn(pkt);
//	}
//
//	return true;
//}
//
//bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
//{
//	if (US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		// TODO : ���� ����? �κ��?
//	}
//
//	return true;
//}
//
//bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
//{
//	if (US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		GameNetwork->HandleSpawn(pkt);
//	}
//
//	return true;
//}
//
//bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
//{
//	if (US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		GameNetwork->HandleDespawn(pkt);
//	}
//
//	return true;
//}
//
//bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
//{
//	if (US1NetworkManager* GameNetwork = GetWorldNetwork(session))
//	{
//		GameNetwork->HandleMove(pkt);
//	}
//
//	return true;
//}
//
//bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
//{
//	auto Msg = pkt.msg();
//
//
//	return true;
//}

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_STC_LOGIN(PacketSessionRef& session, Protocol::STC_LOGIN& pkt)
{
	return false;
}

bool Handle_STC_ENTER_GAME(PacketSessionRef& session, Protocol::STC_ENTER_GAME& pkt)
{
	return false;
}

bool Handle_STC_LEAVE_GAME(PacketSessionRef& session, Protocol::STC_LEAVE_GAME& pkt)
{
	return false;
}

bool Handle_STC_SPAWN(PacketSessionRef& session, Protocol::STC_SPAWN& pkt)
{
	return false;
}

bool Handle_STC_DESPAWN(PacketSessionRef& session, Protocol::STC_DESPAWN& pkt)
{
	return false;
}

bool Handle_STC_MOVE(PacketSessionRef& session, Protocol::STC_MOVE& pkt)
{
	return false;
}

bool Handle_STC_CHAT(PacketSessionRef& session, Protocol::STC_CHAT& pkt)
{
	return false;
}
