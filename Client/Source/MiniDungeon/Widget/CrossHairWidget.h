// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDWidget.h"
#include "CrossHairWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UCrossHairWidget : public UMDWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FVector2D GetScreenMousePosition();
	FVector2D GetAimScreenPosition(UImage* AimImage);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AimPoint;
};
