// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDeadComponent.h"
#include "../Character/MDCharacter.h"


void UHitDeadComponent::PlayHitMontage()
{
	AMDCharacter* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	Character->PlayAnimMontage(HitMontage);
}

void UHitDeadComponent::PlayDeadMontage()
{
	AMDCharacter* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	Character->PlayAnimMontage(DeadMontage);
}
