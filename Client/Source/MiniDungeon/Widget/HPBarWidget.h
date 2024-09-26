// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDWidget.h"
#include "HPBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UHPBarWidget : public UMDWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UProgressBar* ProgressBar;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetHP(int id, float HP);
};
