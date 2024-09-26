// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "../Game/MDGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "../Game/MDGameMode.h"
#include "../Game/MDPlayerState.h"

void UHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));

	AMDGameMode* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
		return;

	/*GameMode->MyPlayerState->OnPlayerHPChanged.AddLambda([&](int id, float amount) -> void
		{
			float PercentValue = amount / 100.0f;
			ProgressBar->SetPercent(PercentValue);
		});*/

	if (IsValid(GameMode->MyPlayerState))
	{
		GameMode->MyPlayerState->OnPlayerHPChanged.AddUObject(this, &UHPBarWidget::SetHP);
	}
}

void UHPBarWidget::SetHP(int id, float HP)
{
	if (ProgressBar == nullptr)
		return;

	float PercentValue = HP / 100.0f;
	ProgressBar->SetPercent(PercentValue);
}
