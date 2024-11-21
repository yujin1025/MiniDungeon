// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerController.h"
#include "MDPlayerState.h"
#include "../Character/MDCharacter.h"
#include <Character/PlayableCharacter.h>

AMDPlayerController::AMDPlayerController()
{
    PlayerInfo = new Protocol::PlayerInfo();
}

void AMDPlayerController::BeginDestroy()
{
    Super::BeginDestroy();

    delete PlayerInfo;
    PlayerInfo = nullptr;
}

void AMDPlayerController::BeginPlay()
{
    MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
    Super::BeginPlay();
    MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AMDPlayerController::PostInitializeComponents()
{
    MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
    Super::PostInitializeComponents();
    MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AMDPlayerController::OnPossess(APawn* aPawn)
{
    MD_LOG(LogMDNetwork, Log, TEXT("%s Begin"), *aPawn->GetName());
    Super::OnPossess(aPawn);
    MD_LOG(LogMDNetwork, Log, TEXT("%s End"), *aPawn->GetName());
    
    if (IsValid(Cast<APlayableCharacter>(aPawn)))
    {
        SetInputMode(FInputModeGameOnly());
    }
}

void AMDPlayerController::OnPossessCharacter(AMDCharacter* aCharacter)
{
    this->OwnerCharacter = aCharacter;
}

void AMDPlayerController::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
    PlayerInfo->CopyFrom(info);
}

AMDPlayerState* AMDPlayerController::GetState()
{
    return Cast<AMDPlayerState>(PlayerState);
}

AMDCharacter* AMDPlayerController::GetCharacter()
{
    return OwnerCharacter;
}
