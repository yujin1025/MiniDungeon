// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "../Character/MDCharacter.h"
#include "../Game/MDGameMode.h"
#include "../Game/MDGameState.h"
#include "../Game/MDPlayerState.h"
#include "Components/WidgetComponent.h"
#include "../Widget/HPBarWidget.h"
#include "../Network/MDNetworkManager.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AMDCharacter* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	ECharacterType CharacterType = Character->GetCharacterType();

	auto* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
		return;

	auto* Data = GameMode->GetCharacterStat(CharacterType);
	if (Data == nullptr)
		return;

	MaxHealth = Data->MaxHp;
	CurrentHealth = MaxHealth;

	UE_LOG(LogTemp, Warning, TEXT("Character %d: MaxHealth set to %f"), Character->CharacterId, Data->MaxHp);
}

void UHealthComponent::ChangeHealth(AMDCharacter* Attacker, float Amount)
{
	auto* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
		return;

	if (CurrentHealth <= 0)
		return;

	CurrentHealth += Amount;

	auto* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	if (Character->IsPlayer()) 
	{
		GameMode->MyPlayerState->OnChangePlayerHealth(CurrentHealth, MaxHealth);
	}
	else
	{
		GameMode->MyGameState->OnChangedHealth(Character->GetObjectID(), CurrentHealth, MaxHealth);
		HPBarWidget = Character->FindComponentByClass<UWidgetComponent>();
		if (HPBarWidget)
		{
			UHPBarWidget* HPWidget = Cast<UHPBarWidget>(HPBarWidget->GetWidget());
			if (HPWidget)
			{
				HPWidget->UpdateHealthBar();
			}
		}
	}

	if (Amount < 0)
	{
		if (CurrentHealth <= 0)
		{
			Character->OnDie();
		}
		else
		{
			Character->OnHit();
		}
	}
}

