// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDCharacter.h"
#include "Protocol.pb.h"
#include "NonPlayableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ANonPlayableCharacter : public AMDCharacter
{
	GENERATED_BODY()
	
public:
	ANonPlayableCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	bool IsFindPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
	TObjectPtr<class APlayableCharacter> TargetPlayer;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HPBarWidget;

	UFUNCTION()
	bool Attack(APlayableCharacter* Player, float hp);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
