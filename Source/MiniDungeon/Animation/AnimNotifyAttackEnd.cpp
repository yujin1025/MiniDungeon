// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyAttackEnd.h"
#include "../Character/MDCharacter.h"

void UAnimNotifyAttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	auto owner = MeshComp->GetOwner();
	if (owner == nullptr)
		return;

	AMDCharacter* character = Cast<AMDCharacter>(owner);
	if (character != nullptr)
	{
		character->OnFinishedSkillMotion(AttackType);
	}
}
