// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Protocol.pb.h"
#include "RoomListViewItemData.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API URoomListViewItemData : public UObject
{
	GENERATED_BODY()
public:
	URoomListViewItemData() : RoomIndex(0), PlayerNum(0), Host(nullptr) { Host = new Protocol::PlayerInfo(); }

	UPROPERTY(EditAnywhere)
	uint64 RoomIndex;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString RoomName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString RoomPassword;

	UPROPERTY(EditAnywhere)
	uint64 PlayerNum;

private:
	Protocol::PlayerInfo* Host;

	TArray<Protocol::PlayerInfo*> Players;

public:
	Protocol::PlayerInfo* GetHost() { return Host; }
	void SetHost(const Protocol::PlayerInfo& info);

	const TArray<Protocol::PlayerInfo*>& GetPlayers() { return Players; }
	void AddPlayer(const Protocol::PlayerInfo& info);
};
