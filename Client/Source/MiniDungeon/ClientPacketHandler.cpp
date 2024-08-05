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
// * 현재 월드의 InstanceSubSystem인 US1NetworkManager를 가져오는 함수 입니다.
// * PIE의 경우 GWorld로 접근할 경우 첫 번째만 접근하는 문제가 존재해서 해결버전입니다.
// */
//US1NetworkManager* GetWorldNetwork(const PacketSessionRef& Session)
//{
//	// 엔진에 존재하는 모든 월드를 순회해서 해당월드에 SubGameInstance의 Session을 통해 비교합니다.
//	// SubGameInstance의 경우 PIE여도 독립적으로 생성되기 때문에 Session도 당연히 분리 됩니다.
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
//	// 로비에서 캐릭터 선택해서 인덱스 전송.
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
//		// TODO : 게임 종료? 로비로?
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
