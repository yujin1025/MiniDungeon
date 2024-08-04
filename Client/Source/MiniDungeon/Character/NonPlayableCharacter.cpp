// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacter.h"
#include "../AI/MDAIController.h"

ANonPlayableCharacter::ANonPlayableCharacter()
{
	AIControllerClass = AMDAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//AutoPossessPlayer = EAutoReceiveInput::Disabled; //빙의하지 않음
}
