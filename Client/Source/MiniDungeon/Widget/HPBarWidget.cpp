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

	auto* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
		return;

	GameMode->MyPlayerState->OnPlayerHPChanged.AddLambda([this](int id, float amount) -> void
		{
			float PercentValue = amount / 100.0f;
			ProgressBar->SetPercent(PercentValue);
		});
}
