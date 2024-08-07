// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/MDNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Game/MDGameInstance.h"
#include "Character/PlayableCharacter.h"
#include "Kismet/GameplayStatics.h"


void UMDNetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConnectToServer();
}

void UMDNetworkManager::ConnectToServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"), false);

	FIPv4Address ip;
	FIPv4Address::Parse(IpAddress, ip);

	TSharedRef<FInternetAddr> internetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	internetAddr->SetIp(ip.Value);
	internetAddr->SetPort(Port);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	bool connected = Socket->Connect(*internetAddr);

	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));
		
		// Session
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		// TEMP : Lobby에서 캐릭터 선택창 등
		//{
		//	Protocol::CTS_LOGIN pkt;
		//	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		//	SendPacket(sendBuffer);
		//}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UMDNetworkManager::DisconnectFromServer()
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	Protocol::CTS_LEAVE_GAME leavePkt;
	SendPacket(leavePkt);

	//if(Socket)
	//{
	//	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get();
	//	socketSubsystem->DestroySocket(Socket);
	//	Socket = nullptr;
	//}
}

void UMDNetworkManager::HandleRecvPackets()
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->HandleRecvPackets();
}

void UMDNetworkManager::SendPacket(SendBufferRef sendBuffer)
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	GameServerSession->SendPacket(sendBuffer);
}

void UMDNetworkManager::HandleSpawn(const Protocol::ObjectInfo& objectInfo, bool isMine)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	// 중복 처리 체크
	const uint64 objectId = objectInfo.object_id();
	if (Players.Find(objectId) != nullptr)
	{
		return;
	}

	FVector spawnLocation(objectInfo.pos_info().x(), objectInfo.pos_info().y(), objectInfo.pos_info().z());

	if (isMine)
	{
		auto* pc = UGameplayStatics::GetPlayerController(this, 0);
		APlayableCharacter* player = Cast<APlayableCharacter>(pc->GetPawn());
		if (player == nullptr)
		{
			return;
		}

		//player->SetPlayerInfo(objectInfo.pos_info());
		MyPlayer = player;
		Players.Add(objectInfo.object_id(), player);
	}
	else
	{
		APlayableCharacter* player = Cast<APlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->OtherPlayerClass, &spawnLocation));
		//player->SetPlayerInfo(objectInfo.pos_info());
		Players.Add(objectInfo.object_id(), player);
	}
}

void UMDNetworkManager::HandleSpawn(const Protocol::STC_ENTER_GAME& enterGamePkt)
{
	//HandleSpawn(enterGamePkt);
}

void UMDNetworkManager::HandleSpawn(const Protocol::STC_SPAWN& spawnPkt)
{
	for(auto& player : spawnPkt.players())
	{
		HandleSpawn(player, false);
	}
}

void UMDNetworkManager::HandleDespawn(uint64 objectId)
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* world = GetWorld();
	if(world == nullptr)
	{
		return;
	}

	/*APlayableCharacter** findActor = Players.Find(objectId);
	if(findActor == nullptr)
	{
		return;
	}

	world->DestroyActor(*findActor);*/
}

void UMDNetworkManager::HandleDespawn(const Protocol::STC_DESPAWN& despawnPkt)
{
	for(auto& objectId : despawnPkt.object_ids())
	{
		HandleDespawn(objectId);
	}
}

void UMDNetworkManager::HandleMove(const Protocol::STC_MOVE& movePkt)
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* world = GetWorld();
	if(world == nullptr)
	{
		return;
	}

	const uint64 objectId = movePkt.info().object_id();

	/*APlayableCharacter** findActor = Players.Find(objectId);
	* if(findActor == nullptr)
	{
		return;
	}

	APlayableCharacter* player = (*findActor);
	if(player->IsMyPlayer())
	{
		return;
	}

	const Protocol::PosInfo& info = movePkt.info();
	//player->SetPlayerInfo(info);
	player->SetDestInfo(Info)*/
}


