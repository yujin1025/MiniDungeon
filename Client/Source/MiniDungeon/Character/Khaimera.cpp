// Fill out your copyright notice in the Description page of Project Settings.


#include "Khaimera.h"
#include "../AI/MDAIController.h"
#include <MDNetworkManager.h>
#include <Game/MDPlayerController.h>
#include <Kismet/GameplayStatics.h>

AKhaimera::AKhaimera()
{
}

void AKhaimera::BeginPlay()
{
    MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
    Super::BeginPlay();
    MD_LOG(LogMDNetwork, Log, TEXT("End"));

    if (AIControllerClass)
    {
        AMDAIController* AIController = Cast<AMDAIController>(GetController());
        if (!AIController)
        {
            auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
            auto pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
            if(IsValid(networkManager))
			{
                if (networkManager->isHost)
                {
                    AIController = GetWorld()->SpawnActor<AMDAIController>(AIControllerClass);
                    AIController->Possess(this);
                    MD_LOG(LogMDNetwork, Log, TEXT("Possess"));
                }
			}
        }
    }
}
