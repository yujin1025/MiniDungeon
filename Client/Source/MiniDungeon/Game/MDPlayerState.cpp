// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerState.h"

AMDPlayerState::AMDPlayerState()
{
}

void AMDPlayerState::OnChangePlayerHealth(int objectId, float Amount)
{
	OnPlayerHPChanged.Broadcast(objectId, Amount);

	if (Amount <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed!"));
	}
}
