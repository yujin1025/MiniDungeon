// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomListViewItemData.h"

void URoomListViewItemData::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
	PlayerInfo->CopyFrom(info);
}
