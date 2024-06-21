// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../Character/MDCharacter.h"
#include "AnimNotifyAttackEnd.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UAnimNotifyAttackEnd : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere)
	EAttackType AttackType;
};
