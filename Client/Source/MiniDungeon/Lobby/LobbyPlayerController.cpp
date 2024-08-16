// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyWidget.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	bShowMouseCursor = true;
	//bEnableClickEvents = true;
	//bEnableMouseOverEvents = true;
	ConstructorHelpers::FClassFinder<UUserWidget> loginWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/WBP_LoginWidget.WBP_LoginWidget_C'"));
	if(loginWidgetClass.Succeeded())
	{
		LoginWidgetClass = loginWidgetClass.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> lobbyWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/WBP_LobbyWidget.WBP_LobbyWidget_C'"));
	if (lobbyWidgetClass.Succeeded())
	{
		LobbyWidgetClass = lobbyWidgetClass.Class;
	}
}

void ALobbyPlayerController::OpenLobbyWidget()
{
	if (IsValid(LobbyWidgetClass))
	{
		LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
		if (IsValid(LobbyWidget))
		{
			LobbyWidget->AddToViewport();
		}
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeUIOnly());

	if(IsValid(LoginWidgetClass))
	{
		LoginWidget = CreateWidget<UUserWidget>(this, LoginWidgetClass);
		if(IsValid(LoginWidget))
		{
			LoginWidget->AddToViewport();
		}
	}
}
