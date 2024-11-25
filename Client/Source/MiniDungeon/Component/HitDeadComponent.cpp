// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDeadComponent.h"
#include "../Character/MDCharacter.h"
#include <Character/PlayableCharacter.h>
#include <MDNetworkManager.h>


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
	if (player->IsLocallyControlled())
	{
		auto networkManager = player->GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		if (networkManager)
		{
			for(auto nextPlayer : networkManager->Players)
			{
				if (nextPlayer.Value->GetObjectID() != player->GetObjectID())
				{
					auto playerController = player->GetController();
					playerController->Possess(nextPlayer.Value);
					break;
				}
			}
		}
		//TO DO : 캐릭터 다 죽었을 때 처리
	}

	Character->PlayAnimMontage(DeadMontage);
}
