// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDeadComponent.h"
#include "../Character/MDCharacter.h"
#include <Character/PlayableCharacter.h>
#include <MDNetworkManager.h>
#include "GameFramework/SpectatorPawn.h"
#include <Game/MDGameMode.h>
#include "Character/NonPlayableCharacter.h"
#include "Game/MDGameState.h"

void UHitDeadComponent::PlayHitMontage()
{
	AMDCharacter* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	Character->PlayAnimMontage(HitMontage);
}

void UHitDeadComponent::PlayDeadMontage()
{
	AMDCharacter* Character = Cast<AMDCharacter>(GetOwner());
	if (Character == nullptr)
		return;

	APlayableCharacter* player = Cast<APlayableCharacter>(Character);
	if (player && player->IsLocallyControlled())
	{
		bool bIsAllDead = true;
		auto networkManager = player->GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		if (networkManager)
		{
			for(auto nextPlayer : networkManager->Players)
			{
				if (nextPlayer.Value->GetObjectID() != player->GetObjectID())
				{
					auto playerController = player->GetController();
					if (playerController)
					{
						playerController->Possess(nextPlayer.Value);
						bIsAllDead = false;
						break;
					}
				}
			}
		}
		//TO DO : 캐릭터 다 죽었을 때 처리

		if (bIsAllDead)
		{
			auto playerController = Cast<APlayerController>(player->GetController());
			if (playerController)
			{
				playerController->ChangeState(NAME_Spectating); // Spectator 모드 전환

				// SpectatorPawn 스폰
				ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>();
				if (SpectatorPawn)
				{
					FVector LastLocation = player->GetActorLocation();
					FRotator LastRotation = player->GetActorRotation();

					SpectatorPawn->SetActorLocation(LastLocation);
					SpectatorPawn->SetActorRotation(LastRotation);

					playerController->Possess(SpectatorPawn); // SpectatorPawn을 Possess
				}

				// UI 입력 모드 활성화
				playerController->SetInputMode(FInputModeUIOnly());
				playerController->bShowMouseCursor = true;
			}
		}

		auto gameMode = Cast<AMDGameMode>(player->GetWorld()->GetAuthGameMode());
		if(gameMode)
		{
			gameMode->ShowDefeatWidget();
		}
	}

	ANonPlayableCharacter* nonPlayableCharacter = Cast<ANonPlayableCharacter>(Character);
	if (nonPlayableCharacter)
	{
		auto networkManager = nonPlayableCharacter->GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		if (networkManager)
		{
			if (networkManager->MonsterInfos.Contains(nonPlayableCharacter->GetObjectID()))
			{
				networkManager->MonsterInfos.Remove(nonPlayableCharacter->GetObjectID());

				if (networkManager->MonsterInfos.Num() == 0)
				{
				}
			}
			else if (networkManager->BossInfos.Contains(nonPlayableCharacter->GetObjectID()))
			{
				networkManager->BossInfos.Remove(nonPlayableCharacter->GetObjectID());
			}


		}
		
		

	}

	Character->PlayAnimMontage(DeadMontage);
}
