// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerState.h"

AMDPlayerState::AMDPlayerState()
{
}

void AMDPlayerState::OnChangePlayerHealth(float Amount, float maxHP)
{
	OnPlayerHPChanged.Broadcast(Amount, maxHP);

	if (Amount <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed!"));
	}
}
