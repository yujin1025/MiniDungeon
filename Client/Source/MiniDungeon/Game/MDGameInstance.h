// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Network/MDNetworkManager.h"
#include "MDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMDGameInstance();

protected:
	virtual void Init() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlayableCharacter> AuroraClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlayableCharacter> DrongoClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ANonPlayableCharacter> KhaimeraClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ANonPlayableCharacter> GruxClass;
};
