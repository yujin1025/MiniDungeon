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
#include "Character/Khaimera.h"
#include "AI/MDAIController.h"
#include <Game/MDGameMode.h>
#include "Component/HealthComponent.h"

FRWLock PlayerMapLock;
FRWLock MonsterLock;

void UMDNetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConnectToServer();
}

void UMDNetworkManager::Deinitialize()
{
	Super::Deinitialize();

	DisconnectFromServer();
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
			isHost = true;
			pc->CreateRoom(createRoomPkt.room_info(), true);
		}
		else
		{
			pc->CreateRoom(createRoomPkt.room_info(), false);
		}
	}

	RoomID = createRoomPkt.room_info().room_id();
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
		if(joinRoomPkt.player().player_id() == pc->GetPlayerInfo()->player_id())
		{
			pc->JoinRoom(joinRoomPkt.room_info(), true);
		}
		else
		{
			pc->JoinRoom(joinRoomPkt.room_info(), false);
		}
	}

	RoomID = joinRoomPkt.room_info().room_id();
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
		if(leaveRoomPkt.room_info().host().player_id() == pc->GetPlayerInfo()->player_id())
		{
			isHost = true;
		}
		else
		{
			isHost = false;
			RoomID = 0;
		}

		pc->LeaveRoom(leaveRoomPkt);
	}
}

void UMDNetworkManager::HandleSpawn(const Protocol::ObjectInfo& objectInfo, const Protocol::PlayerType charactertype, TArray<AActor*> spawns, bool isMine)
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

	FVector spawnLocation = spawns[objectId % 4]->GetActorLocation();

	if (isMine)
	{
		AMDPlayerController* pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		APlayableCharacter* player = nullptr;
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

			player->SetObjectID(objectId);
			MyPlayer = player;
			Players.Add(objectId, player);

			if (IsValid(pc))
			{
				pc->GetPawn()->Destroy();
				pc->OnPossess(player);
				MD_LOG(LogMDNetwork, Log, TEXT("Possess To Character"));
			}
			else
			{
				MD_LOG(LogMDNetwork, Log, TEXT("PlayerController is not valid"));
			}

			if(IsValid(gameMode))
			{
				gameMode->MyPlayerState = pc->GetState();
			}
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

		player->SetObjectID(objectId);
		Players.Add(objectInfo.object_id(), player);
	}
}

void UMDNetworkManager::HandleSpawn(const Protocol::PlayerInfo& playerInfo, TArray<AActor*> spawns, bool isMine)
{
	auto pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(IsValid(pc) && isMine)
	{
		pc->SetPlayerInfo(playerInfo);
	}

	HandleSpawn(playerInfo.object_info(), playerInfo.player_type(), spawns, isMine);
}

void UMDNetworkManager::HandleSpawn(const Protocol::ObjectInfo& objectInfo, FVector spawnLocation)
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

	// 중복 처리 체크
	const uint64 objectId = objectInfo.object_id();
	if(Monsters.Find(objectId) != nullptr)
	{
		return;
	}

	ANonPlayableCharacter* monster = Cast<ANonPlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->KhaimeraClass, &spawnLocation));
	monster->SetObjectID(objectId);
	Monsters.Add(objectId, monster);

	Protocol::CTS_MOVE movePkt;
	Protocol::PosInfo* info = new Protocol::PosInfo();
	info->set_object_id(objectId);
	info->set_x(spawnLocation.X);
	info->set_y(spawnLocation.Y);
	info->set_z(spawnLocation.Z);
	info->set_yaw(0);

	movePkt.set_allocated_info(info);
	SendPacket(ClientPacketHandler::MakeSendBuffer(movePkt));
}

void UMDNetworkManager::HandleSpawnBoss(const Protocol::ObjectInfo& objectInfo, FVector spawnLocation)
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

	const uint64 objectId = objectInfo.object_id();
	if (Boss.Find(objectId) != nullptr)
	{
		return;
	}

	ANonPlayableCharacter* monster = Cast<ANonPlayableCharacter>(world->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->GruxClass, &spawnLocation));
	monster->SetObjectID(objectId);
	Boss.Add(objectId, monster);

	Protocol::CTS_MOVE movePkt;
	Protocol::PosInfo* info = new Protocol::PosInfo();
	info->set_object_id(objectId);
	info->set_x(spawnLocation.X);
	info->set_y(spawnLocation.Y);
	info->set_z(spawnLocation.Z);
	info->set_yaw(0);

	movePkt.set_allocated_info(info);
	SendPacket(ClientPacketHandler::MakeSendBuffer(movePkt));
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

	// TODO : DESPAWN 처리

	//auto findCharacter = Players.Find(objectId);
	//if(findCharacter)
	//{
	//	Players.Remove(objectId);
	//	world->DestroyActor(*findCharacter);
	//	return;;
	//}

	//auto findMonster = Monsters.Find(objectId);
	//if(findMonster)
	//{
	//	MonsterInfos.Remove(objectId);
	//	Monsters.Remove(objectId);
	//	world->DestroyActor(*findMonster);
	//	return;
	//}

	//TODO : BOSS DESPAWN 처리
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

	if(Players.Contains(objectId))
	{
		if (IsValid(Players[objectId]))
		{
			HandleMovePlayer(*Players.Find(objectId), movePkt.info());
			return;
		}
	}

	if(Monsters.Contains(objectId))
	{
		if (IsValid(Monsters[objectId]))
		{
			HandleMoveMonster(*Monsters.Find(objectId), movePkt.info(), movePkt.target_object_id());
			return;
		}
	}

	//TODO : BOSS MOVE 처리
	if(Boss.Contains(objectId))
	{
		if (IsValid(Boss[objectId]))
		{
			HandleMoveMonster(*Boss.Find(objectId), movePkt.info(), movePkt.target_object_id());
			return;
		}
	}
}

void UMDNetworkManager::HandleMovePlayer(APlayableCharacter* player, const Protocol::PosInfo& posInfo)
{
	if(player == nullptr)
	{
		return;
	}

	if (player->IsMyPlayer())
	{
		return;
	}

	//이동 정보 가져와서 업데이트 
	player->SetPosInfo(posInfo);
	player->SetDestInfo(posInfo);
}

void UMDNetworkManager::HandleMoveMonster(ANonPlayableCharacter* monster, const Protocol::PosInfo& posInfo, uint64 target_object_id)
{
	// TODO : Monster AI MOVE
	auto aiController = Cast<AMDAIController>(monster->GetController());
	if(aiController)
	{
		if(target_object_id == 0)
			aiController->CustomMoveToLocation(FVector(posInfo.x(), posInfo.y(), posInfo.z()));
		else
		{
			TObjectPtr<APlayableCharacter>* findPlayer = Players.Find(target_object_id);
			if (findPlayer)
			{
				APlayableCharacter* player = *findPlayer;
				aiController->CustomMoveToActor(player);
			}
		}
	}
}

void UMDNetworkManager::HandleAttack(const Protocol::STC_ATTACK& AtkPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = AtkPkt.info().attack_object_id();

	TObjectPtr<APlayableCharacter>* findActor = Players.Find(ObjectId);
	if (findActor == nullptr)
		return;

	APlayableCharacter* player = (*findActor);

	const Protocol::AttackInfo& Info = AtkPkt.info();
	player->Other_Attack(Info);
}

void UMDNetworkManager::HandleMonsterAttack(const Protocol::STC_MONSTER_ATTACK& pkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	TWeakObjectPtr<ANonPlayableCharacter> WeakMonster;
	TWeakObjectPtr<APlayableCharacter> WeakPlayer;
	if (Monsters.Contains(pkt.monster_id()))
	{
		WeakMonster = Monsters.Find(pkt.monster_id())->Get();
	}

	if(Boss.Contains(pkt.monster_id()))
	{
		WeakMonster = Boss.Find(pkt.monster_id())->Get();
	}
	
	if(!pkt.attacked_infos().empty())
	{
		if (Players.Contains(pkt.attacked_infos().begin()->attacked_object_id()))
		{
			WeakPlayer = Players.Find(pkt.attacked_infos().begin()->attacked_object_id())->Get();
		}
	}

	if (WeakMonster.IsValid() && WeakPlayer.IsValid())
	{
		ANonPlayableCharacter* monster = WeakMonster.Get();
		APlayableCharacter* player = WeakPlayer.Get();

		if (monster && player)
		{
			monster->SetActorRotation(monster->GetTargetRotation(player->GetActorLocation()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Monster or Player is invalid"));
		}
	}


	for (auto attackedInfo : pkt.attacked_infos())
	{
		TMap<uint64, float> attackedObjectInfos;
		if (Players.Contains(attackedInfo.attacked_object_id()))
		{
			APlayableCharacter* player = Players.Find(attackedInfo.attacked_object_id())->Get();
			if(player)
			{
				if (player->HealthComponent)
				{
					player->HealthComponent->ChangeHealth(attackedInfo.attacked_object_current_hp());
				}
			}
		}

		if (static_cast<EAttackType>(pkt.monster_attack_type()) < EAttackType::Max)
		{
			if (WeakMonster.IsValid())
				WeakMonster.Get()->Attack(static_cast<EAttackType>(pkt.monster_attack_type()));
		}
	}
}

void UMDNetworkManager::HandleAttacked(const Protocol::STC_ATTACKED& pkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 objectId = pkt.attacking_object_id();
	TObjectPtr<APlayableCharacter>* PlayerPtr = Players.Find(objectId);

	if (PlayerPtr == nullptr || !IsValid(*PlayerPtr))
		return;

	APlayableCharacter* Player = *PlayerPtr;

	TWeakObjectPtr<APlayableCharacter> WeakPlayer = Players.Find(objectId)->Get();
	TWeakObjectPtr<ANonPlayableCharacter> WeakMonster;

	for (const auto& attacked_info : pkt.attacked_infos())
	{
		TObjectPtr<ANonPlayableCharacter>* MonsterPtr = Monsters.Find(attacked_info.attacked_object_id());
		TObjectPtr<ANonPlayableCharacter>* BossPtr = Boss.Find(attacked_info.attacked_object_id());

		TObjectPtr<ANonPlayableCharacter> TargetMonster = (MonsterPtr != nullptr) ? *MonsterPtr : (BossPtr != nullptr) ? *BossPtr : nullptr;

		if (IsValid(TargetMonster))
		{
			if (TargetMonster->HealthComponent)
			{
				TargetMonster->HealthComponent->ChangeHealth(attacked_info.attacked_object_current_hp());
			}
		}
	}

	if (static_cast<EAttackType>(pkt.attacking_skill_type()) < EAttackType::Max)
	{
		if (IsValid(Player))
		{
			Player->UseSkill(pkt.attacking_player_type(), static_cast<EAttackType>(pkt.attacking_skill_type()));
		}
	}
}

void UMDNetworkManager::HandleSpawnBoss(const Protocol::MonsterInfo& bossInfo)
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	AddBossInfo(bossInfo.object_info().object_id(), bossInfo);

     auto gameMode = Cast<AMDGameMode>(World->GetAuthGameMode());
	 if(gameMode)
	 {
		 gameMode->SpawnBoss(bossInfo.object_info(), FVector(1000.0f, 1000.0f, 100.0f));
	 }
}

void UMDNetworkManager::AddPlayerInfo(uint64 player_id, const Protocol::PlayerInfo& info)
{
	if(PlayerInfos.Contains(player_id))
	{
		PlayerInfos[player_id] = new Protocol::PlayerInfo(info);
	}
	else
	{
		PlayerInfos.Add(player_id, new Protocol::PlayerInfo(info));
	}
}

void UMDNetworkManager::AddMonsterInfo(uint64 object_id, const Protocol::MonsterInfo& info)
{
	if(MonsterInfos.Contains(object_id))
	{
		MonsterInfos[object_id] = new Protocol::MonsterInfo(info);
	}
	else
	{
		MonsterInfos.Add(object_id, new Protocol::MonsterInfo(info));
	}
}

void UMDNetworkManager::AddBossInfo(uint64 object_id, const Protocol::MonsterInfo& info)
{
	if(BossInfos.Contains(object_id))
	{
		BossInfos[object_id] = new Protocol::MonsterInfo(info);
	}
	else
	{
		BossInfos.Add(object_id, new Protocol::MonsterInfo(info));
	}
}


