// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "MDGameMode.generated.h"

class AMDGameState;
class AMDPlayerState;
class AMDPlayerController;
class APlayerController;
/**
 * 
 */

UENUM()
enum class ECharacterType
{
	Aurora = 0,
	Drongo = 1,
	Khaimera = 2,
	Grux = 3,
	Max,
};

USTRUCT(BlueprintType)
struct FCharacterStatData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCharacterStatData() : MaxHp(100.0f), MoveSpeed(2.0f) {}

	UPROPERTY(EditAnywhere)
	float MaxHp;

	UPROPERTY(EditAnywhere)
	float MoveSpeed;
};


UCLASS()
class MINIDUNGEON_API AMDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMDGameMode();

	virtual void BeginPlay() override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	virtual void OnPostLogin(AController* NewPlayer) override;


public:
	UPROPERTY()
	AMDGameState* MyGameState;

	UPROPERTY()
	AMDPlayerState* MyPlayerState;

private:
	UPROPERTY(EditAnywhere, Category = Data)
	class UDataTable* CharacterStatTable;

	//UI
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UMDWidget> ingameWindowWidgetClass;

	UPROPERTY()
	UMDWidget* ingameWindowWidget;

public:
	FCharacterStatData* GetCharacterStat(ECharacterType type);

};
