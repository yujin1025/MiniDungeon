// Fill out your copyright notice in the Description page of Project Settings.


#include "Grux.h"
#include "../AI/MDAIController.h"

AGrux::AGrux()
{
}

void AGrux::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));

    if (AIControllerClass)
    {
        auto AIController = GetWorld()->SpawnActor<AMDAIController>(AIControllerClass);
        AIController->Possess(this);
        MD_LOG(LogMDNetwork, Log, TEXT("Poss"));
    }
}
