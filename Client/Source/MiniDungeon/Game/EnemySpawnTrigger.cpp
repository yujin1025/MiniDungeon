// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/EnemySpawnTrigger.h"
#include "Components/BoxComponent.h"
#include <Character/PlayableCharacter.h>
#include "MDGameMode.h"
#include "MDNetworkManager.h"
#include <Kismet/GameplayStatics.h>
#include <Game/MDPlayerController.h>

// Sets default values
AEnemySpawnTrigger::AEnemySpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetCollisionProfileName("Trigger");
}

// Called when the game starts or when spawned
void AEnemySpawnTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (Trigger->OnComponentBeginOverlap.IsBound())
	{
		Trigger->OnComponentBeginOverlap.Clear();
	}

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnTrigger::OnCharacterOverlap);
}

void AEnemySpawnTrigger::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(Cast<APlayableCharacter>(OtherActor)))
	{
		/*auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		auto pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (networkManager->HostID == pc->GetPlayerInfo()->player_id())
		{
			AMDGameMode* GameMode = Cast<AMDGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(GameMode))
			{
				GameMode->SpawnEnemy();
			}
		}*/
	}
}

// Called every frame
void AEnemySpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

