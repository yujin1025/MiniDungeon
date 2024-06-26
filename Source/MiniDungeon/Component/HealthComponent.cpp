// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "../Character/MDCharacter.h"
#include "../Game/MDGameMode.h"
#include "../Game/MDGameState.h"
#include "../Game/MDPlayerState.h"

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

	auto* Data = GameMode->GetCharacterStat(ECharacterType::Aurora);
	if (Data == nullptr)
		return;

	MaxHealth = Data->MaxHp;
	CurrentHealth = MaxHealth;

	UE_LOG(LogTemp, Warning, TEXT("Character %d: MaxHealth set to %f"), Character->CharacterId, Data->MaxHp);
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
		GameMode->MyPlayerState->OnChangePlayerHealth(Character->CharacterId, CurrentHealth);
		UE_LOG(LogTemp, Warning, TEXT("Player Number : (%d) Current Health: %f"), Character->CharacterId, CurrentHealth);
	}
	else
	{
		GameMode->MyGameState->OnChangedHealth(Character->CharacterId, CurrentHealth);
		UE_LOG(LogTemp, Warning, TEXT("Non Player Number : (%d) Current Health: %f"), Character->CharacterId, CurrentHealth);
	}

	UE_LOG(LogTemp, Warning, TEXT("Character %d: CurrentHealth is now %f"), Character->CharacterId, CurrentHealth);

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
		OnDamaged.Broadcast(Attacker, CurrentHealth);
	}
}

