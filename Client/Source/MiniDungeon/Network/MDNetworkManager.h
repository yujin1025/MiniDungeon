// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "MiniDungeon.h"
#include "Protocol.pb.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MDNetworkManager.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDNetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	UFUNCTION(BlueprintCallable)
	void ConnectToServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef sendBuffer);

	template<typename T>
	void SendPacket(T& packet) const;

public:
	void HandleEmailSent();
	void HandleEmailSendFail();
	void HandleAuthSuccess();
	void HandleAuthFail();
	void HandleDuplicateID(bool isDuplicated);
	void HandleSignUpSuccess();
	void HandleSignUpFail();
	void HandleLogin(const Protocol::STC_ENTER_LOBBY& enterLobbyPkt);
	void HandleCreateRoom(const Protocol::STC_CREATE_ROOM& createRoomPkt);
	void HandleJoinRoom(const Protocol::STC_JOIN_ROOM& joinRoomPkt);
	void HandleChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt);
	void HandleLeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt);

	void HandleSpawn(const Protocol::ObjectInfo& objectInfo, const Protocol::PlayerType charactertype, TArray<AActor*> spawns, bool isMine);
	void HandleSpawn(const Protocol::PlayerInfo& playerInfo, TArray<AActor*> spawns, bool isMine);
	void HandleSpawn(const Protocol::ObjectInfo& objectInfo, FVector spawnLocation = FVector::ZeroVector);

	void HandleDespawn(uint64 objectId);
	void HandleDespawn(const Protocol::STC_DESPAWN& despawnPkt);

	void HandleMove(const Protocol::STC_MOVE& movePkt);
	void HandleMovePlayer(class APlayableCharacter* player, const Protocol::PosInfo& posInfo);
	void HandleMoveMonster(class ANonPlayableCharacter* monster, const Protocol::PosInfo& posInfo, uint64 target_object_id = 0);

	void HandleAttack(const Protocol::STC_ATTACK& AtkPkt);
	void HandleMonsterAttack(uint64 attacking_obj_id, uint64 attacked_obj_id);

	void HandleSpawnMonster(const Protocol::STC_MONSTERINFO& InfoPkt);
	void HandleMonsterInfo(const Protocol::STC_MONSTERINFO& infoPkt);

public:
	class FSocket* Socket;
	//FString IpAddress = TEXT("43.202.241.72");
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	TSharedPtr<class PacketSession> GameServerSession;

public:
	/// <summary>
	/// Key : ObjectID
	/// Value : PlayerCharacter
	/// </summary>
	TMap<uint64,TObjectPtr<class APlayableCharacter>> Players;

	/// <summary>
	/// Key : PlayerID
	/// Value : PlayerInfo
	/// </summary>
	TMap<uint64, Protocol::PlayerInfo*> PlayerInfos;
	void AddPlayerInfo(uint64 player_id, const Protocol::PlayerInfo& info);

	TObjectPtr<class APlayableCharacter> MyPlayer;

	uint64 PlayerID;

	Protocol::PosInfo* PosInfo;
	const Protocol::PosInfo* GetPosInfo() { return PosInfo; }

	/// <summary>
	/// Key : ObjectID
	/// Value : MonsterInfo
	/// </summary>
	TMap<uint64, Protocol::MonsterInfo*> MonsterInfos;
	void AddMonsterInfo(uint64 object_id, const Protocol::MonsterInfo& info);
	/// <summary>
	/// Key : ObjectID
	/// Value : MonsterCharacter
	/// </summary>
	TMap<uint64, TObjectPtr<class ANonPlayableCharacter>> Monsters;

	bool isHost = false;

	uint64 RoomID;
};

template<typename T>
void UMDNetworkManager::SendPacket(T& packet) const
{
	if(Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	const SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
	GameServerSession->SendPacket(sendBuffer);
}