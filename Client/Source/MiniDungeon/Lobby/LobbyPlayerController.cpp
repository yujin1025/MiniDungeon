// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyWidget.h"
#include "Lobby/LoginWidget.h"
#include "Lobby/RoomWidget.h"
#include "Lobby/RoomListViewItemData.h"
#include <Blueprint/WidgetLayoutLibrary.h>

ALobbyPlayerController::ALobbyPlayerController()
{
	bShowMouseCursor = true;
	//bEnableClickEvents = true;
	//bEnableMouseOverEvents = true;
	ConstructorHelpers::FClassFinder<UUserWidget> loginWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/Lobby/WBP_LoginWidget.WBP_LoginWidget_C'"));
	if(loginWidgetClass.Succeeded())
	{
		LoginWidgetClass = loginWidgetClass.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> lobbyWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/Lobby/WBP_LobbyWidget.WBP_LobbyWidget_C'"));
	if (lobbyWidgetClass.Succeeded())
	{
		LobbyWidgetClass = lobbyWidgetClass.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> roomWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/Lobby/WBP_RoomWidget.WBP_RoomWidget_C'"));
	if (roomWidgetClass.Succeeded())
	{
		RoomWidgetClass = roomWidgetClass.Class;
	}

	PlayerInfo = new Protocol::PlayerInfo();
}

ALobbyPlayerController::~ALobbyPlayerController()
{
	delete PlayerInfo;
	PlayerInfo = nullptr;
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeUIOnly());

	if(IsValid(LoginWidgetClass))
	{
		LoginWidget = CreateWidget<ULoginWidget>(this, LoginWidgetClass);
		if(IsValid(LoginWidget))
		{
			LoginWidget->AddToViewport();
		}
	}
}

void ALobbyPlayerController::OpenLobbyWidget()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);

	if (IsValid(LobbyWidgetClass))
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (IsValid(LobbyWidget))
		{
			Cast<ULobbyWidget>(LobbyWidget)->Owner = this;
			LobbyWidget->AddToViewport();
			LobbyWidget->RefreshListView(RoomList);
		}
	}
}

void ALobbyPlayerController::HandleOpenLobbyWidget(const Protocol::STC_ENTER_LOBBY& enterLobbyPkt)
{
	SetPlayerInfo(enterLobbyPkt.player());

	for (auto& room : enterLobbyPkt.rooms())
	{
		UpdateRoomData(room);
	}

	OpenLobbyWidget();
}

URoomListViewItemData* ALobbyPlayerController::AddRoomData(const Protocol::RoomInfo& info)
{
	URoomListViewItemData* roomData = NewObject<URoomListViewItemData>();

	roomData->SetInfo(info);

	FString roomName = UTF8_TO_TCHAR(info.room_name().c_str());

	if (RoomList.Contains(roomName))
	{
		RoomList.Remove(roomName);
	}

	RoomList.Add(roomName, roomData);

	return roomData;
}

URoomListViewItemData* ALobbyPlayerController::UpdateRoomData(const Protocol::RoomInfo& info)
{
	const FString roomName = UTF8_TO_TCHAR(info.room_name().c_str());

	if (!RoomList.Contains(roomName))
	{
		AddRoomData(info);
	}
	else
	{
		RoomList[roomName]->SetInfo(info);
	}

	return RoomList[roomName];
}

void ALobbyPlayerController::CreateRoom(const Protocol::RoomInfo& info, bool isHost)
{
	auto roomData = UpdateRoomData(info);

	if (isHost)
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
		if (IsValid(RoomWidgetClass))
		{
			RoomWidget = CreateWidget<URoomWidget>(this, RoomWidgetClass);
			if (IsValid(RoomWidget))
			{
				RoomWidget->SetRoomData(roomData);
				RoomWidget->AddToViewport();
			}
		}
	}
	else
	{
		if (IsValid(LobbyWidget))
		{
			LobbyWidget->RefreshListView(RoomList);
		}
	}
}

void ALobbyPlayerController::JoinRoom(const Protocol::RoomInfo& info)
{
	auto roomData = UpdateRoomData(info);

	UWidgetLayoutLibrary::RemoveAllWidgets(this);
	if (IsValid(RoomWidgetClass))
	{
		RoomWidget = CreateWidget<URoomWidget>(this, RoomWidgetClass);
		if (IsValid(RoomWidget))
		{
			const FString roomName = UTF8_TO_TCHAR(info.room_name().c_str());
			RoomWidget->SetRoomData(roomData);
			RoomWidget->AddToViewport();
		}
	}
}

void ALobbyPlayerController::LeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt)
{
	RoomList.Empty();
	for (const auto& roomData : leaveRoomPkt.rooms())
	{
		UpdateRoomData(roomData);
	}

	if (PlayerInfo->player_id()  == leaveRoomPkt.player_id())
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
		OpenLobbyWidget();	
		return;
	}

	if (IsValid(RoomWidget))
	{
		const FString roomName = UTF8_TO_TCHAR(leaveRoomPkt.room_info().room_name().c_str());
		if (RoomList.Contains(roomName))
		{
			RoomWidget->SetRoomData(RoomList[roomName]);
			RoomWidget->HandleLeaveRoom();
		}
	}
}

void ALobbyPlayerController::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
	PlayerInfo->CopyFrom(info);
}

void ALobbyPlayerController::ChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt)
{
	if(IsValid(LobbyWidget))
	{
		LobbyWidget->GetRoomWidget()->ChangeCharacterImage(changeCharacterPkt.player_id(), changeCharacterPkt.character(), false);
	}
}
