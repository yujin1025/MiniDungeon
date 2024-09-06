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

}

void ULobbyWidget::CreateRoom(const uint64 roomIndex, const FString& roomName, const FString& password, const Protocol::PlayerInfo& info)
{
	URoomListViewItemData* roomData = NewObject<URoomListViewItemData>();

	roomData->RoomIndex = roomIndex;
	roomData->RoomName = roomName;
	roomData->RoomPassword = password;
	roomData->SetHost(info);
	roomData->AddPlayer(info);

	RoomList.Add(roomData);

	RoomListView->AddItem(roomData);

	if (IsValid(RoomWidgetClass))
	{
		RoomWidget = CreateWidget<URoomWidget>(this, RoomWidgetClass);
		if (IsValid(RoomWidget))
		{
			RoomWidget->SetRoomData(roomData);	
			RoomWidget->AddToViewport();
		}
	}

	//RefreshListView();
}

void ULobbyWidget::RemoveRoom(const uint64 roomIndex)
{
}

void ULobbyWidget::UpdateRoom(const uint64 roomIndex, const uint64 playerNum)
{
}

void ULobbyWidget::ClearRoomList()
{
}

void ULobbyWidget::RefreshListView()
{

}

void ULobbyWidget::OnClickedCreateButton()
{
	Protocol::CTS_CREATE_ROOM createRoomPkt;

	Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
	playerInfo->CopyFrom(*Owner->GetPlayerInfo());
	createRoomPkt.set_allocated_player(playerInfo);
	
	createRoomPkt.set_roomname(TCHAR_TO_UTF8(*(RoomNameInput->GetText().ToString())));
	createRoomPkt.set_password(TCHAR_TO_UTF8(*(PasswordInput->GetText().ToString())));
	
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(createRoomPkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void ULobbyWidget::OnClickedJoinButton()
{
}
