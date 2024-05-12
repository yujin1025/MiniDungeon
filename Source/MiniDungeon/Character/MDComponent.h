// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "MDCharacter.h"
#include "MDComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIDUNGEON_API UMDComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMDComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*
	MDCharacter* GetCyphersCharacter()
	{
		auto owner = this->GetOwner();
		if (owner == nullptr)
			return nullptr;

		return Cast<MDCharacter>(owner);
	}*/

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
