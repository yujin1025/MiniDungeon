// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameInstance.h"
#include "Character/Aurora.h"
#include "Character/Drongo.h"
#include "Character/Khaimera.h"
#include "Character/Grux.h"

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

	ConstructorHelpers::FClassFinder<AKhaimera> KhaimeraBP(TEXT("/Script/Engine.Blueprint'/Game/Assets/BluePrints/NPC/BP_Khaimera.BP_Khaimera_C'"));
	if (KhaimeraBP.Succeeded())
	{
		KhaimeraClass = KhaimeraBP.Class;
	}

	ConstructorHelpers::FClassFinder<AGrux> GruxBP(TEXT("/Script/Engine.Blueprint'/Game/Assets/BluePrints/NPC/BP_Grux.BP_Grux_C'"));
	if (GruxBP.Succeeded())
	{
		GruxClass = GruxBP.Class;
	}
}

void UMDGameInstance::Init()
{
	Super::Init();
}
