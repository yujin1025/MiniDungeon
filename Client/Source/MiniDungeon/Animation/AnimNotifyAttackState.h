// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyAttackState.generated.h"

class AMDCharacter;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UAnimNotifyAttackState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyAttackState();

	void TryAttack(USkeletalMeshComponent* MeshComp);
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	AMDCharacter* GetCharacter(const FOverlapResult& OverlapResult);
	bool TryGetOverlapResult(AMDCharacter* Owner, TArray<FOverlapResult>& OverlapResults);
	bool TryGetOverlapTarget(AMDCharacter* Owner, OUT AMDCharacter*& FoundTarget);

protected:
	UPROPERTY(EditAnywhere)
	int MaxAttackCount = 1;

	int CurrentAttackCount = 0;

	UPROPERTY(EditAnywhere)
	float DamageAmount;

	UPROPERTY(EditAnywhere)
	float DetectX = 600.0f;
};
