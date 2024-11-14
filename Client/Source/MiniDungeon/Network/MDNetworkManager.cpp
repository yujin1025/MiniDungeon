// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/MDNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "Game/MDGameInstance.h"
#include "Character/Aurora.h"
#include "Character/Drongo.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyPlayerController.h"
#include <Lobby/RoomListViewItemData.h>
#include "Game/MDPlayerController.h"
#include "../Character/Khaimera.h"
#include "../AI/MDAIController.h"
#include <Game/MDGameMode.h>


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

void UMDNetworkManager::HandleEmailSent()
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if(IsValid(pc))
	{
		pc->OpenAuthWidget();
	}
}

void UMDNetworkManager::HandleEmailSendFail()
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
	
	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));


}

void UMDNetworkManager::HandleAuthSuccess()
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (IsValid(pc))
	{
		pc->OpenSignUpWidget();
	}
}

void UMDNetworkManager::HandleAuthFail()
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UMDNetworkManager::HandleDuplicateID(bool isDuplicated)
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if(IsValid(pc))
	{
		pc->OnIDChecked(isDuplicated);
	}

}

void UMDNetworkManager::HandleSignUpSuccess()
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		pc->OnRegistered();
	}
}

void UMDNetworkManager::HandleSignUpFail()
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void UMDNetworkManager::HandleLogin(const Protocol::STC_ENTER_LOBBY& enterLobbyPkt)
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

	PlayerID = enterLobbyPkt.player().player_id();

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		pc->HandleOpenLobbyWidget(enterLobbyPkt);
	}
}

void UMDNetworkManager::HandleCreateRoom(const Protocol::STC_CREATE_ROOM& createRoomPkt)
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		if (pc->GetPlayerInfo()->player_id() == createRoomPkt.room_info().host().player_id())
		{
			pc->CreateRoom(createRoomPkt.room_info(), true);
		}
		else
		{
			pc->CreateRoom(createRoomPkt.room_info(), false);
		}
	}
}

void UMDNetworkManager::HandleJoinRoom(const Protocol::STC_JOIN_ROOM& joinRoomPkt)
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		pc->JoinRoom(joinRoomPkt.room_info());
	}
}

void UMDNetworkManager::HandleChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt)
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		pc->ChangeCharacter(changeCharacterPkt);
	}

}

void UMDNetworkManager::HandleLeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt)
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

	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (IsValid(pc))
	{
		pc->LeaveRoom(leaveRoomPkt);
	}

}

void UMDNetworkManager::HandleSpawn(const Protocol::ObjectInfo& objectInfo, const Protocol::PlayerType charactertype, bool isMine)
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

	if (!isMine)
	{
		spawnLocation += FVector(15, 0, 0); 
	}

	if (isMine)
	{
		AMDPlayerController* pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		APlayableCharacter* player = Cast<APlayableCharacter>(pc->GetPawn());
		AMDGameMode* gameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());

		if (true)
		{
			switch (charactertype)
			{
			case Protocol::PLAYER_TYPE_AURORA:
				player = Cast<APlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->AuroraClass, &spawnLocation));
				MD_LOG(LogMDNetwork, Log, TEXT("Spawn Character"));
				break;
			case Protocol::PLAYER_TYPE_DRONGO:
				player = Cast<APlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->DrongoClass, &spawnLocation));
				MD_LOG(LogMDNetwork, Log, TEXT("Spawn Character"));
				break;
			}

			if (IsValid(pc))
			{
				pc->OnPossess(player);
				MD_LOG(LogMDNetwork, Log, TEXT("Possess To Character"));
			}

			if(IsValid(gameMode))
			{
				gameMode->MyPlayerState = pc->GetState();
			}

			MyPlayer = player;
			Players.Add(objectInfo.object_id(), player);
		}
	}
	else
	{
		APlayableCharacter* player = nullptr;
		switch (charactertype)
		{
		case Protocol::PLAYER_TYPE_AURORA:
			player = Cast<APlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->AuroraClass, &spawnLocation));
			MD_LOG(LogMDNetwork, Log, TEXT("Spawn Character"));
			break;
		case Protocol::PLAYER_TYPE_DRONGO:
			player = Cast<APlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->DrongoClass, &spawnLocation));
			MD_LOG(LogMDNetwork, Log, TEXT("Spawn Character"));
			break;
		}

		Players.Add(objectInfo.object_id(), player);
	}
}

void UMDNetworkManager::HandleSpawn(const Protocol::PlayerInfo& playerInfo, bool isMine)
{
	HandleSpawn(playerInfo.object_info(), playerInfo.player_type(), isMine);
}

void UMDNetworkManager::HandleSpawn(const Protocol::STC_ENTER_GAME& enterGamePkt)
{
	for (auto& player : enterGamePkt.players())
	{
		if(player.player_id() == PlayerID)
		{
			HandleSpawn(player, true);
		}
		else
		{
			HandleSpawn(player, false);
		}
	}
}

void UMDNetworkManager::HandleSpawn(const Protocol::STC_SPAWN& spawnPkt)
{
	//for(auto& player : spawnPkt.players())
	//{
	//	HandleSpawn(player, player  false);
	//}
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

	//이동하려는 플레이어 식별
	const uint64 objectId = movePkt.info().object_id();

	TObjectPtr<APlayableCharacter>* findActor = Players.Find(objectId);
	if(findActor == nullptr)
	{
		return;
	}

	APlayableCharacter* player = (*findActor);
	if(player->IsMyPlayer())
	{
		return;
	}

	//이동 정보 가져와서 업데이트 
	const Protocol::PosInfo& info = movePkt.info();
	player->SetPlayerInfo(info);
	player->SetDestInfo(info);
	MD_LOG(LogMDNetwork, Log, TEXT("PlayerID: %llu"), info.object_id());
}

void UMDNetworkManager::HandleAttack(const Protocol::STC_ATTACK& AtkPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = AtkPkt.info().object_id();

	TObjectPtr<APlayableCharacter>* findActor = Players.Find(ObjectId);
	if (findActor == nullptr)
		return;

	APlayableCharacter* player = (*findActor);

	const Protocol::AttackInfo& Info = AtkPkt.info();
	player->Other_Attack(Info);
}

void UMDNetworkManager::HandleSpawnMonster(const Protocol::STC_MONSTERINFO& InfoPkt)
{
	auto* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	const Protocol::MonsterInfo& monsterInfo = InfoPkt.info();
	uint64 objectId = monsterInfo.object_info().object_id();

	FVector spawnLocation(1000.0f, 1000.0f, 100.0f);
	ANonPlayableCharacter* npc = Cast<ANonPlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->KhaimeraClass, &spawnLocation));
	
	Monsters.Add(objectId, npc);
	MD_LOG(LogMDNetwork, Log, TEXT("Spawn Character"));
}

void UMDNetworkManager::HandleMonsterInfo(const Protocol::STC_MONSTERINFO& infoPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	Protocol::MonsterInfo Info = infoPkt.info();
	const uint64 ObjectId = Info.object_info().object_id();

	// 보스 찾기
	TObjectPtr<ANonPlayableCharacter>* MonsterPtr = Monsters.Find(ObjectId);
	if (MonsterPtr == nullptr)
		return;

	ANonPlayableCharacter* Monster = *MonsterPtr;
	Monster->MaxHP = Info.monster_hp();
	Monster->CurrentHP = Monster->MaxHP;
	Monster->Speed = Info.speed();
	Monster->Damage = Info.damage();
	Monster->IsFindPlayer = Info.isfindplayer();

	// 타겟 플레이어 찾기
	TObjectPtr<APlayableCharacter>* FindPlayer = Players.Find(Info.targetplayer_id());
	if (FindPlayer == nullptr)
		return;

	APlayableCharacter* Player = *FindPlayer;
	Monster->TargetPlayer = Player;

	if (Monster->AIControllerClass)
	{
		AMDAIController* AIController = Cast<AMDAIController>(Monster->AIControllerClass);
		if (AIController)
		{
			AIController->SetBlackboardValues(Monster->IsFindPlayer, Monster->TargetPlayer, Monster->TargetPlayer->GetActorLocation(), Monster->Speed, Info.calcdist());
		}
	}
}


