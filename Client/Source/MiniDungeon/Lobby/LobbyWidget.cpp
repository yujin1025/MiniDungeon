// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>
#include "Protocol.pb.h"
#include "LobbyPlayerController.h"
#include "RoomListViewItemData.h"
#include "Components/ListView.h"
#include "RoomWidget.h"

ULobbyWidget::ULobbyWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> roomWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/Lobby/WBP_RoomWidget.WBP_RoomWidget_C'"));
	if (roomWidgetClass.Succeeded())
	{
		RoomWidgetClass = roomWidgetClass.Class;
	}
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(CreateButton))
	{
		CreateButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickedCreateButton);
	}

	if (IsValid(JoinButton))
	{
		JoinButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickedJoinButton);
	}

	RefreshListView();
}

class URoomListViewItemData* ULobbyWidget::AddRoomData(const Protocol::RoomInfo& info)
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

URoomListViewItemData* ULobbyWidget::UpdateRoomData(const Protocol::RoomInfo& info)
{
	RoomListView->ClearListItems();

	const FString roomName = UTF8_TO_TCHAR(info.room_name().c_str());

	if(!RoomList.Contains(roomName))
	{
		AddRoomData(info);
	}
	else
	{
		RoomList[roomName]->SetInfo(info);
	}

	for (auto& room : RoomList)
	{
		RoomListView->AddItem(room.Value);
	}

	return RoomList[roomName];
}

void ULobbyWidget::CreateRoom(const Protocol::RoomInfo& info, bool isHost)
{
	auto roomData = UpdateRoomData(info);

	if(isHost)
	{
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
}

void ULobbyWidget::JoinRoom(const Protocol::RoomInfo& info)
{
	auto roomData = UpdateRoomData(info);

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

void ULobbyWidget::HandleLeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt)
{
	if (Owner->GetPlayerInfo()->player_id() == leaveRoomPkt.player_id())
	{
		if (IsValid(RoomWidget))
		{
			RoomWidget->RemoveFromParent();
		}

		RoomList.Empty();
		for (const auto& roomData : leaveRoomPkt.rooms())
		{
			UpdateRoomData(roomData);
		}

		return;
	}

	RoomList.Empty();
	for (const auto& roomData : leaveRoomPkt.rooms())
	{
		UpdateRoomData(roomData);
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

void ULobbyWidget::RemoveRoom(const uint64 roomIndex)
{
}

void ULobbyWidget::UpdateRoom(const uint64 roomIndex, const uint64 playerNum)
{
	
	if (IsValid(RoomWidget))
	{
		
	}
}

void ULobbyWidget::ClearRoomList()
{
	RoomList.Empty();
	RoomListView->ClearListItems();
}

void ULobbyWidget::RefreshListView()
{
	for(auto &roomData : RoomList)
	{
		RoomListView->AddItem(roomData.Value);
	}
}

void ULobbyWidget::OnClickedCreateButton()
{
	Protocol::CTS_CREATE_ROOM createRoomPkt;

	Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();

	const FString& roomName = RoomNameInput->GetText().ToString();
	const FString& password = PasswordInput->GetText().ToString();

	roomInfo->set_room_id(0);
	roomInfo->set_room_name(TCHAR_TO_UTF8(*roomName));
	roomInfo->set_password(TCHAR_TO_UTF8(*password));
	roomInfo->set_current_player_count(1);

	Protocol::PlayerInfo* host = new Protocol::PlayerInfo();
	host->CopyFrom(*Owner->GetPlayerInfo());
	roomInfo->set_allocated_host(host);

	roomInfo->add_players()->CopyFrom(*Owner->GetPlayerInfo());

	createRoomPkt.set_allocated_room_info(roomInfo);
	
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(createRoomPkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void ULobbyWidget::OnClickedJoinButton()
{
	Protocol::CTS_JOIN_ROOM joinRoomPkt;

	const FString& roomName = RoomNameInput->GetText().ToString();
	const FString& password = PasswordInput->GetText().ToString();

	const auto roomData = RoomList.Find(roomName);

	if (roomData == nullptr)
	{
		// TODO : 방이 없음.
		return;
	}

	joinRoomPkt.set_roomindex((*roomData)->RoomIndex);
	joinRoomPkt.set_password(TCHAR_TO_UTF8(*password));

	Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
	playerInfo->CopyFrom(*Owner->GetPlayerInfo());
	joinRoomPkt.set_allocated_player(playerInfo);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(joinRoomPkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

