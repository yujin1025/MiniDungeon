#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "MDNetworkManager.h"
#include "Game/MDGameInstance.h"
#include "MiniDungeon.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

/*
 * 현재 월드의 InstanceSubSystem인 US1NetworkManager를 가져오는 함수 입니다.
 * PIE의 경우 GWorld로 접근할 경우 첫 번째만 접근하는 문제가 존재해서 해결버전입니다.
 */

UMDNetworkManager* GetWorldNetwork(const PacketSessionRef& Session)
{
	// 엔진에 존재하는 모든 월드를 순회해서 해당월드에 SubGameInstance의 Session을 통해 비교합니다.
	// SubGameInstance의 경우 PIE여도 독립적으로 생성되기 때문에 Session도 당연히 분리 됩니다.
	for(auto World : GEngine->GetWorldContexts())
	{
		if(const UGameInstance* gameInstance = World.World()->GetGameInstance())
		{
			if(UMDNetworkManager* NetworkManager = gameInstance->GetSubsystem<UMDNetworkManager>())
			{
				if (NetworkManager->GameServerSession == Session)
				{
					return NetworkManager;
				}
			}
		}
	}

	return nullptr;
}

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_STC_LOGIN(PacketSessionRef& session, Protocol::STC_LOGIN& pkt)
{
	for (auto& Player : pkt.players())
	{
	}

	for (int32 i = 0; i < pkt.players_size(); i++)
	{
		const Protocol::ObjectInfo& player = pkt.players(i);
	}

	// 로비에서 캐릭터 선택해서 인덱스 전송.
	Protocol::CTS_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerindex(0);

	if (const UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		gameNetwork->SendPacket(enterGamePkt);
	}

	return true;
}
//
bool Handle_STC_ENTER_GAME(PacketSessionRef& session, Protocol::STC_ENTER_GAME& pkt)
{
	if (UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		gameNetwork->HandleSpawn(pkt);
	}

	return true;
}
//
bool Handle_STC_LEAVE_GAME(PacketSessionRef& session, Protocol::STC_LEAVE_GAME& pkt)
{
	if (UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		// TODO : 게임 종료? 로비로?
	}

	return true;
}
//
bool Handle_STC_SPAWN(PacketSessionRef& session, Protocol::STC_SPAWN& pkt)
{
	if (UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		gameNetwork->HandleSpawn(pkt);
	}

	return true;
}
//
bool Handle_STC_DESPAWN(PacketSessionRef& session, Protocol::STC_DESPAWN& pkt)
{
	if (UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		gameNetwork->HandleDespawn(pkt);
	}

	return true;
}
//
bool Handle_STC_MOVE(PacketSessionRef& session, Protocol::STC_MOVE& pkt)
{
	if (UMDNetworkManager* gameNetwork = GetWorldNetwork(session))
	{
		gameNetwork->HandleMove(pkt);
	}

	return true;
}
//
bool Handle_STC_CHAT(PacketSessionRef& session, Protocol::STC_CHAT& pkt)
{
	auto Msg = pkt.msg();


	return true;
}
