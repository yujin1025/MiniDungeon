// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerController.h"
#include "MDPlayerState.h"

AMDPlayerState* AMDPlayerController::GetState()
{
    return Cast<AMDPlayerState>(PlayerState);
}
