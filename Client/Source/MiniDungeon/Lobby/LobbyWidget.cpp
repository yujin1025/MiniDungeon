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

TObjectPtr<class URoomListViewItemData> ULobbyWidget::AddRoomData(const Protocol::RoomInfo& info)
{
	URoomListViewItemData* roomData = NewObject<URoomListViewItemData>();

	roomData->SetInfo(info);

	RoomList.Add(roomData);

	RoomListView->AddItem(roomData);

	return roomData;
}

void ULobbyWidget::CreateRoom(const Protocol::RoomInfo& info, bool isHost)
{
	auto roomData = AddRoomData(info);

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

void ULobbyWidget::RemoveRoom(const uint64 roomIndex)
{
}

void ULobbyWidget::UpdateRoom(const uint64 roomIndex, const uint64 playerNum)
{
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
		RoomListView->AddItem(roomData);
	}
}

void ULobbyWidget::OnClickedCreateButton()
{
	Protocol::CTS_CREATE_ROOM createRoomPkt;

	Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
	roomInfo->set_room_name(TCHAR_TO_UTF8(*(RoomNameInput->GetText().ToString())));
	roomInfo->set_password(TCHAR_TO_UTF8(*(PasswordInput->GetText().ToString())));
	roomInfo->set_current_player_count(1);

	Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
	playerInfo->CopyFrom(*Owner->GetPlayerInfo());
	roomInfo->set_allocated_host(playerInfo);

	createRoomPkt.set_allocated_room_info(roomInfo);
	
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(createRoomPkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void ULobbyWidget::OnClickedJoinButton()
{
}
