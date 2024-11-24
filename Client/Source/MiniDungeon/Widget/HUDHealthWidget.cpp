// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDHealthWidget.h"
#include "Components/ProgressBar.h"
#include "../Game/MDGameMode.h"
#include "../Game/MDPlayerState.h"

void UHUDHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));

	AMDGameMode* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
		return;

	if (IsValid(GameMode->MyPlayerState))
	{
		GameMode->MyPlayerState->OnPlayerHPChanged.AddUObject(this, &UHUDHealthWidget::SetHP);
	}
}

void UHUDHealthWidget::SetHP(int id, float HP)
{
	if (ProgressBar == nullptr)
		return;

	float PercentValue = HP / 100.0f;
	ProgressBar->SetPercent(PercentValue);
}
