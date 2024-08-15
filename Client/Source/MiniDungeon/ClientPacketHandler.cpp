#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "MDNetworkManager.h"
#include "Game/MDGameInstance.h"
#include "MiniDungeon.h"
#include "Struct.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

/*
 * ���� ������ InstanceSubSystem�� NetworkManager�� �������� �Լ� �Դϴ�.
 * PIE�� ��� GWorld�� ������ ��� ù ��°�� �����ϴ� ������ �����ؼ� �ذ�����Դϴ�.
 */

UMDNetworkManager* GetWorldNetwork(const PacketSessionRef& Session)
{
	// ������ �����ϴ� ��� ���带 ��ȸ�ؼ� �ش���忡 SubGameInstance�� Session�� ���� ���մϴ�.
	// SubGameInstance�� ��� PIE���� ���������� �����Ǳ� ������ Session�� �翬�� �и� �˴ϴ�.
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv STC_LOGIN Packet")));
	if(pkt.success() == false)
	{
		// �α��� ����
		return false;
	}

	// �α��� ����
	const UMDNetworkManager* gameNetwork = GetWorldNetwork(session);

	if (gameNetwork != nullptr)
	{
		UMDNetworkManager* nonConstGameNetwork = const_cast<UMDNetworkManager*>(gameNetwork);

	}
	// �κ� ����
	Protocol::CTS_ENTER_LOBBY enterLobbyPkt;

	if (gameNetwork != nullptr)
	{
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(pkt.player());

		enterLobbyPkt.set_allocated_player(playerInfo);
		gameNetwork->SendPacket(enterLobbyPkt);
	}


	// �κ񿡼� ĳ���� �����ؼ� �ε��� ����.
	/*Protocol::CTS_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerindex(0);

	if (gameNetwork)
	{
		gameNetwork->SendPacket(enterGamePkt);
	}*/

	return true;
}
bool Handle_STC_ENTER_LOBBY(PacketSessionRef& session, Protocol::STC_ENTER_LOBBY& pkt)
{
	return false;
}

bool Handle_STC_JOIN_OR_CREATE_ROOM(PacketSessionRef& session, Protocol::STC_JOIN_OR_CREATE_ROOM& pkt)
{
	return false;
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
		// TODO : ���� ����? �κ��?
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
