// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameInstance.h"
#include "Character/Aurora.h"
#include "Character/Drongo.h"

UMDGameInstance::UMDGameInstance()
{
	ConstructorHelpers::FClassFinder<AAurora> AuroraBP(TEXT("/Script/Engine.Blueprint'/Game/Assets/BluePrints/PC/BP_Aurora.BP_Aurora_C'"));
	if (AuroraBP.Succeeded())
	{
		AuroraClass = AuroraBP.Class;
	}	
	
	ConstructorHelpers::FClassFinder<ADrongo> DrongoBP(TEXT("/Script/Engine.Blueprint'/Game/Assets/BluePrints/PC/BP_Drongo.BP_Drongo_C'"));
	if (DrongoBP.Succeeded())
	{
		DrongoClass = DrongoBP.Class;
	}

}
