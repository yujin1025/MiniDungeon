// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameState.h"

AMDGameState::AMDGameState()
{
}

void AMDGameState::OnChangedHealth(int ObjectID, float Amount)
{
	if (Amount <= 0.0f)
	{
		AActor* Actor = GetOwner();
		if (Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Monster Destroyed!"));
		}
	}
}
