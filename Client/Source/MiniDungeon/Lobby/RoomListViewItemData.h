// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoomListViewItemData.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API URoomListViewItemData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	uint64 RoomIndex;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FText RoomName;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FText RoomPassword;
	UPROPERTY(EditAnywhere)
	uint64 PlayerNum;
};
