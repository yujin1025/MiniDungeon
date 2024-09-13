// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerController.h"
#include "MDPlayerState.h"
#include "../Character/MDCharacter.h"
#include <Character/PlayableCharacter.h>

void AMDPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AMDPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
    OnPossessCharacter(Cast<APlayableCharacter>(aPawn));
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
