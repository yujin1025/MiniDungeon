// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameState.h"
#include "../Character/MDCharacter.h"
#include "../Character/Khaimera.h"
#include "../Character/Grux.h"
#include "../Character/PlayableCharacter.h"
#include "MDPlayerState.h"
#include "MDPlayerController.h"

AMDGameState::AMDGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMDGameState::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AMDGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}