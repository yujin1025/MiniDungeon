// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MDWidget.h"
#include "HUDHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UHUDHealthWidget : public UMDWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UProgressBar* ProgressBar;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetHP(float currentHP, float HP);
};
