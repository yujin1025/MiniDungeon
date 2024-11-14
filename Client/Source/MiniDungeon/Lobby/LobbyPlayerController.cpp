// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyWidget.h"
#include "Lobby/LoginWidget.h"
#include "Lobby/RoomWidget.h"
#include "Lobby/RoomListViewItemData.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "AuthWidget.h"
#include "SignUpWidget.h"

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
		if (!IsValid(LobbyWidget))
		{
			LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		}
	}

	if (IsValid(LobbyWidget))
	{
		LobbyWidget->Owner = this;
		LobbyWidget->AddToViewport();
		LobbyWidget->RefreshListView(RoomList);
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

void ALobbyPlayerController::OpenAuthWidget()
{
	if (IsValid(LoginWidget))
	{
		LoginWidget->OepnAuthWidget();
	}
}

void ALobbyPlayerController::OpenSignUpWidget()
{
	if(IsValid(LoginWidget))
	{
		LoginWidget->OpenSignUpWidget();
	}
}

void ALobbyPlayerController::OnIDChecked(bool isDuplicated)
{
	if(IsValid(LoginWidget))
	{
		LoginWidget->OnIDChecked(isDuplicated);
	}
}

void ALobbyPlayerController::OnRegistered()
{
	if(IsValid(LoginWidget))
	{
		LoginWidget->OnRegistered();
	}
}

void ALobbyPlayerController::CloseAllPopupWidget()
{
	if (IsValid(LoginWidget))
	{
		LoginWidget->CloseAllPopupWidget();
	}
}

void ALobbyPlayerController::CreateRoom(const Protocol::RoomInfo& info, bool isHost)
{
	auto roomData = UpdateRoomData(info);

	if (isHost)
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
		if (IsValid(RoomWidgetClass))
		{
			if (!IsValid(RoomWidget))
			{
				RoomWidget = CreateWidget<URoomWidget>(this, RoomWidgetClass);
			}
		}

		if (IsValid(RoomWidget))
		{
			RoomWidget->SetRoomData(roomData);
			RoomWidget->AddToViewport();
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

void ALobbyPlayerController::JoinRoom(const Protocol::RoomInfo& info, bool isJoin)
{
	auto roomData = UpdateRoomData(info);

	if (isJoin)
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
		if (IsValid(RoomWidgetClass))
		{
			if (!IsValid(RoomWidget))
				RoomWidget = CreateWidget<URoomWidget>(this, RoomWidgetClass);
		}

		if (IsValid(RoomWidget))
		{
			RoomWidget->SetRoomData(roomData);
			RoomWidget->AddToViewport();
		}
	}
	else
	{
		if (IsValid(RoomWidget))
		{
			RoomWidget->SetRoomData(roomData);
			RoomWidget->RefreshPlayers();
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

	// Lobby를 나간 플레이어일 경우
	if (PlayerInfo->player_id() == leaveRoomPkt.player_id())
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
		RoomWidget->SetRoomData(nullptr);
		OpenLobbyWidget();	
		return;
	}

	// Room에 있는 플레이어일 경우
	if (IsValid(RoomWidget) && IsValid(RoomWidget->GetRoomData()))
	{
		const FString roomName = UTF8_TO_TCHAR(leaveRoomPkt.room_info().room_name().c_str());
		if (RoomList.Contains(roomName))
		{
			RoomWidget->SetRoomData(RoomList[roomName]);
			RoomWidget->HandleLeaveRoom();
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

void ALobbyPlayerController::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
	PlayerInfo->CopyFrom(info);
}

void ALobbyPlayerController::ChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt)
{
	if(IsValid(RoomWidget))
	{
		RoomWidget->ChangeCharacterImage(changeCharacterPkt.player_id(), changeCharacterPkt.character(), false);
	}
}
