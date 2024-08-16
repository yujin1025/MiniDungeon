// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerController.h"
#include "Lobby/LobbyWidget.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	bShowMouseCursor = true;
	//bEnableClickEvents = true;
	//bEnableMouseOverEvents = true;
	ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/UI/WBP_Lobby.WBP_Lobby_C'"));
	if(WidgetClass.Succeeded())
	{
		LobbyWidgetClass = WidgetClass.Class;
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeUIOnly());

	if(IsValid(LobbyWidgetClass))
	{
		LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
		if(IsValid(LobbyWidget))
		{
			LobbyWidget->AddToViewport();
		}
	}
}
