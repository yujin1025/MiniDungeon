// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class AMDCharacter;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDamaged, AMDCharacter*, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIDUNGEON_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

private:
	float MaxHealth;
	float CurrentHealth;

public:
	FOnDamaged OnDamaged;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ChangeHealth(AMDCharacter* Attacker, float Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }
		
};
