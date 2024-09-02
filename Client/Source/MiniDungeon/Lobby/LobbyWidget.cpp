// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>
#include "Protocol.pb.h"
#include "LobbyPlayerController.h"

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

void ULobbyWidget::RefreshListView()
{
	
}

void ULobbyWidget::OnClickedCreateButton()
{
	Protocol::CTS_CREATE_ROOM createRoomPkt;

	Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
	playerInfo->CopyFrom(*Owner->GetPlayerInfo());
	createRoomPkt.set_allocated_player(playerInfo);
	
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(createRoomPkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void ULobbyWidget::OnClickedJoinButton()
{
}
