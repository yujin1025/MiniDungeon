// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "Aurora.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AAurora : public APlayableCharacter
{
	GENERATED_BODY()
	
public:
	AAurora();

	virtual void BeginPlay() override;

protected:
	virtual void OnUseQSkill() override;
	virtual void OnUseESkill() override;
	virtual void OnUseShiftSkill() override;
};
