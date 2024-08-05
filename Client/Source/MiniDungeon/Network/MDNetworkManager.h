// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClientPacketHandler.h"
//#include "PacketSession.h"
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
	UFUNCTION(BlueprintCallable)
	void ConnectToServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromServer();

	UFUNCTION(BlueprintCallable)

	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	template<typename T>
	void SendPacket(T& Packet) const;

public:


public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	//TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY()
	TMap<uint64,TObjectPtr<class APlayableCharacter>> Players;

	UPROPERTY()
	TObjectPtr<class APlayableCharacter> MyPlayer;
};

template<typename T>
void UMDNetworkManager::SendPacket(T& Packet) const
{
	if(Socket == nullptr /*|| GameServerSession == nullptr*/)
	{
		return;
	}

	const SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Packet);
	//GameServerSession->SendPacket(SendBuffer);
}