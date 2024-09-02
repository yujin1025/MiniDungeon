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

	void HandleSpawn(const Protocol::ObjectInfo& objectInfo, bool isMine);
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

	UPROPERTY()
	TObjectPtr<class APlayableCharacter> MyPlayer;
	UPROPERTY()
	FString PlayerID;
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