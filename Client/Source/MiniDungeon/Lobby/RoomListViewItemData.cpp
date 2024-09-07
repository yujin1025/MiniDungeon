// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomListViewItemData.h"

URoomListViewItemData::URoomListViewItemData()
{
	Host = new Protocol::PlayerInfo();
	Info = new Protocol::RoomInfo();
}

URoomListViewItemData::~URoomListViewItemData()
{
	delete Host;
	Host = nullptr;

	for (auto player : Players)
	{
		delete player;
	}
	Players.Empty();

	delete Info;
	Info = nullptr;
}

void URoomListViewItemData::SetHost(const Protocol::PlayerInfo& info)
{
	Host->CopyFrom(info);
}

void URoomListViewItemData::AddPlayer(const Protocol::PlayerInfo& info)
{
	Protocol::PlayerInfo* player = new Protocol::PlayerInfo();
	player->CopyFrom(info);
	Players.Add(player);
}

void URoomListViewItemData::SetInfo(const Protocol::RoomInfo& info)
{
	Info->CopyFrom(info);

	RoomIndex = Info->room_id();
	RoomName = UTF8_TO_TCHAR(Info->room_name().c_str());
	RoomPassword = UTF8_TO_TCHAR(Info->password().c_str());
	PlayerNum = Info->current_player_count();

	AddPlayer(Info->host());
	SetHost(Info->host());
}
