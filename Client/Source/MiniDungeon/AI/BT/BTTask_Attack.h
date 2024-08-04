#pragma once

#include "CoreMinimal.h"
#include "MDTaskNode.h"
#include "../../Character/MDCharacter.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UBTTask_Attack : public UMDTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	EAttackType AttackType;

	bool bIsProcessing = false;
};
