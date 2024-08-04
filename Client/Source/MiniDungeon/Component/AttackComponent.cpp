// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComponent.h"
#include "../Character/MDCharacter.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttackComponent::PlayAttackMontage()
{
	AMDCharacter* character = Cast<AMDCharacter>(GetOwner());
	if (character == nullptr)
		return;

	character->PlayAnimMontage(AttackMontage);
}



void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	
}



void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

