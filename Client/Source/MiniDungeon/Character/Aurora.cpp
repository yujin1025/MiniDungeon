// Fill out your copyright notice in the Description page of Project Settings.


#include "Aurora.h"
#include "MiniDungeon.h"

AAurora::AAurora()
{
}

void AAurora::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AAurora::OnUseQSkill()
{
}

void AAurora::OnUseESkill()
{
}

void AAurora::OnUseShiftSkill()
{
}
