// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "../../Character/MDCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	bIsProcessing = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	auto* Character = GetCharacter(OwnerComp);
	if (Character == nullptr)
		return EBTNodeResult::Failed;


	Character->UseSkill(AttackType);
	bIsProcessing = true;
	Character->OnUseSkillDelegate.AddLambda([this](EAttackType AttackType) -> void
	{
		bIsProcessing = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bIsProcessing)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
