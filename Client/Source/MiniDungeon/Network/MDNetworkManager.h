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
	void HandleLogin(const Protocol::STC_ENTER_LOBBY& enterLobbyPkt);
	void HandleCreateRoom(const Protocol::STC_CREATE_ROOM& createRoomPkt);
	void HandleJoinRoom(const Protocol::STC_JOIN_ROOM& joinRoomPkt);
	void HandleChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt);
	void HandleLeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt);

	void HandleSpawn(const Protocol::ObjectInfo& objectInfo, const Protocol::PlayerType charactertype, bool isMine);
	void HandleSpawn(const Protocol::PlayerInfo& playerInfo, bool isMine);
	void HandleSpawn(const Protocol::STC_ENTER_GAME& enterGamePkt);
	void HandleSpawn(const Protocol::STC_SPAWN& spawnPkt);

	void HandleDespawn(uint64 objectId);
	void HandleDespawn(const Protocol::STC_DESPAWN& despawnPkt);

	void HandleMove(const Protocol::STC_MOVE& movePkt);

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY()
	TMap<uint64,TObjectPtr<class APlayableCharacter>> Players;

	TMap<uint64, TSharedPtr<Protocol::PlayerInfo>> PlayerInfos;

	UPROPERTY()
	TObjectPtr<class APlayableCharacter> MyPlayer;
	UPROPERTY()
	uint64 PlayerID;
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