// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyAttackState.h"
#include "../Component/HealthComponent.h"
#include "../Character/MDCharacter.h"
#include <Character/NonPlayableCharacter.h>
#include "Network/MDNetworkManager.h"


UAnimNotifyAttackState::UAnimNotifyAttackState()
{

}

void UAnimNotifyAttackState::TryAttack(USkeletalMeshComponent* MeshComp)
{
	if (CurrentAttackCount >= MaxAttackCount)
		return;

	AMDCharacter* MyCharacter = Cast<AMDCharacter>(MeshComp->GetOwner());
	AMDCharacter* TargetCharacter = nullptr;

	/*if (TryGetOverlapTarget(MyCharacter, TargetCharacter))
	{
		UHealthComponent* DamagedHealthComponent = TargetCharacter->FindComponentByClass<UHealthComponent>();

		if (DamagedHealthComponent)
		{
			DamagedHealthComponent->ChangeHealth(MyCharacter, -DamageAmount);
			CurrentAttackCount++;
		}
	}*/
}

void UAnimNotifyAttackState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	CurrentAttackCount = 0;
	TryAttack(MeshComp);
}

void UAnimNotifyAttackState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	TryAttack(MeshComp);
}

void UAnimNotifyAttackState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	CurrentAttackCount = 0;
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
}

AMDCharacter* UAnimNotifyAttackState::GetCharacter(const FOverlapResult& OverlapResult)
{
	auto* Actor = OverlapResult.GetActor();
	if (Actor == nullptr)
		return nullptr;

	return Cast<AMDCharacter>(Actor);
}

bool UAnimNotifyAttackState::TryGetOverlapResult(AMDCharacter* Owner, TArray<FOverlapResult>& OverlapResults)
{
	auto* World = Owner ? Owner->GetWorld() : nullptr;
	if (World == nullptr)
		return false;

	FVector Center = Owner->GetActorLocation();
	FCollisionQueryParams CollisionParam(NAME_None, false, Owner);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(600.0f),
		CollisionParam);

	DrawDebugSphere(World, Center, DetectX, 16, FColor::Red, false, 0.2f);
	return bResult;
}

bool UAnimNotifyAttackState::TryGetOverlapTarget(AMDCharacter* Owner, OUT AMDCharacter*& FoundTarget)
{
	TArray<FOverlapResult> OverlapResults;
	bool bResult = false;

	if (TryGetOverlapResult(Owner, OverlapResults))
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			auto* TargetCharacter = GetCharacter(OverlapResult);
			if (TargetCharacter == nullptr)
				continue;

			if (TargetCharacter->IsPlayer() == Owner->IsPlayer())
				continue;

			bResult = true;
			FoundTarget = TargetCharacter;
			break;
		}
	}

	return bResult;
}
