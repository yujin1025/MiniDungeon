// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyWidget.h"
#include "Lobby/LoginWidget.h"

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

	PlayerInfo = new Protocol::PlayerInfo();
}

ALobbyPlayerController::~ALobbyPlayerController()
{
	delete PlayerInfo;
	PlayerInfo = nullptr;
}

void ALobbyPlayerController::OpenLobbyWidget(const Protocol::STC_ENTER_LOBBY& enterLobbypkt)
{
	SetPlayerInfo(enterLobbypkt.player());

	if (IsValid(LobbyWidgetClass))
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (IsValid(LobbyWidget))
		{
			Cast<ULobbyWidget>(LobbyWidget)->Owner = this;
			LobbyWidget->AddToViewport();

			for(auto& room : enterLobbypkt.rooms())
			{
				LobbyWidget->AddRoomData(room);
			}
		}
	}
}

void ALobbyPlayerController::JoinRoom(const Protocol::RoomInfo& info)
{
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->JoinRoom(info);
	}
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

void ALobbyPlayerController::CreateRoom(const Protocol::RoomInfo& info, bool isHost)
{
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->CreateRoom(info, isHost);
	}
}

void ALobbyPlayerController::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
	PlayerInfo->CopyFrom(info);
}

void ALobbyPlayerController::ChangeCharacter(const Protocol::PlayerType& type)
{
	PlayerInfo->set_player_type(type);
}
