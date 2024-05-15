// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyAttackEnd.h"
#include "../Character/MDCharacter.h"

void UAnimNotifyAttackEnd::Notify(USkeletalMeshComponent* MeshComp)
{
	auto owner = MeshComp->GetOwner();
	if (owner == nullptr)
		return;

	AMDCharacter* character = Cast<AMDCharacter>(owner);
	if (character != nullptr)
	{
		character->OnFinishedSkillMotion(AttackType);
		UE_LOG(LogTemp, Warning, TEXT("OnFinishedSkillMotion."));
	}
}
