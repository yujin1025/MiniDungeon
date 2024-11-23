// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"
#include "Components/ProgressBar.h"
#include "../Game/MDGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "../Component/HealthComponent.h"
#include "../Character/MDCharacter.h"


void UHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
}

void UHPBarWidget::UpdateHealthBar()
{
	if (ProgressBar && HealthComponent)
	{
		float HealthPercent = HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth();
		ProgressBar->SetPercent(HealthPercent);
	}
}

void UHPBarWidget::SetHealthComponent(UHealthComponent* NewHealthComponent)
{
	HealthComponent = NewHealthComponent;
}

