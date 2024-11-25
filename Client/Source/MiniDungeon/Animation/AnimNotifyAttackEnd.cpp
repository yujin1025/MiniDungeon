// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyAttackEnd.h"
#include "../Character/MDCharacter.h"
#include <Character/NonPlayableCharacter.h>
#include "Network/MDNetworkManager.h"

void UAnimNotifyAttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	auto owner = MeshComp->GetOwner();
	if (owner == nullptr)
		return;

	ANonPlayableCharacter* monster = Cast<ANonPlayableCharacter>(MeshComp->GetOwner());

	if (IsValid(monster))
	{
		// TWeakObjectPtr로 monster를 약한 참조로 캡처
		TWeakObjectPtr<ANonPlayableCharacter> weakMonster = monster;
		FTimerHandle timerHandle;

		weakMonster.Get()->GetWorld()->GetTimerManager().SetTimer(
			timerHandle,
			[weakMonster]()
			{
				if (weakMonster.IsValid())
				{
					ANonPlayableCharacter* monsterPtr = weakMonster.Get();

					Protocol::CTS_MONSTER_ATTACK monsterAttackPkt;
					monsterAttackPkt.set_monster_id(monsterPtr->GetObjectID());
					monsterAttackPkt.set_monster_attack_type(static_cast<uint64>(EAttackType::QSkillAttack));

					SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(monsterAttackPkt);

					// 네트워크 매니저 가져오기
					auto networkManager = monsterPtr->GetGameInstance()->GetSubsystem<UMDNetworkManager>();
					if (IsValid(networkManager))
					{
						networkManager->SendPacket(sendBuffer);
					}
				}
			},
			1.0f, false
		);
	}

	AMDCharacter* character = Cast<AMDCharacter>(owner);
	if (character != nullptr)
	{
		character->OnFinishedSkillMotion(AttackType);
	}
}
