// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomListViewItemData.h"

void URoomListViewItemData::SetHost(const Protocol::PlayerInfo& info)
{
	Host->CopyFrom(info);
}

void URoomListViewItemData::AddPlayer(const Protocol::PlayerInfo& info)
{
	Protocol::PlayerInfo* player = new Protocol::PlayerInfo();
	player->CopyFrom(info);
	Players.AddUnique(player);
}
