// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlayableCharacter> OtherPlayerClass;
};
