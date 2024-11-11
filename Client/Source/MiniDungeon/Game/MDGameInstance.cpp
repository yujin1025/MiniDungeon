// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameInstance.h"
#include "Character/Aurora.h"
#include "Character/Drongo.h"
#include "Character/Khaimera.h"

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

}

void UMDGameInstance::Init()
{
	Super::Init();

	// UMDNetworkManager를 서브시스템으로 초기화
	NetworkManager = NewObject<UMDNetworkManager>(this);
	if (NetworkManager)
	{
		NetworkManager->AddToRoot(); // 가비지 컬렉션 방지
		UE_LOG(LogTemp, Log, TEXT("UMDNetworkManager initialized successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize UMDNetworkManager."));
	}
}
