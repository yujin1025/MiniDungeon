// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"
#include "Lobby/LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	// Set the default pawn class to be our character
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}
