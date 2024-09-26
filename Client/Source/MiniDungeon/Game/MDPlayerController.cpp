// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerController.h"
#include "MDPlayerState.h"
#include "../Character/MDCharacter.h"
#include <Character/PlayableCharacter.h>

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

AMDPlayerState* AMDPlayerController::GetState()
{
    return Cast<AMDPlayerState>(PlayerState);
}

AMDCharacter* AMDPlayerController::GetCharacter()
{
    return OwnerCharacter;
}
