// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TESTPlayerController.h"
#include "MiniDungeon.h"

void ATESTPlayerController::PostInitializeComponents()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::PostInitializeComponents();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void ATESTPlayerController::OnPossess(APawn* aPawn)
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::OnPossess(aPawn);
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

