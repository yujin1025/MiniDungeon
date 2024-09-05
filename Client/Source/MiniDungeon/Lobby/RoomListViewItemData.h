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
	URoomListViewItemData() : RoomIndex(0), PlayerNum(0), PlayerInfo(nullptr) { PlayerInfo = new Protocol::PlayerInfo(); }

	UPROPERTY(EditAnywhere)
	uint64 RoomIndex;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString RoomName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString RoomPassword;

	UPROPERTY(EditAnywhere)
	uint64 PlayerNum;

private:
	Protocol::PlayerInfo* PlayerInfo;

public:
	Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& info);
};
