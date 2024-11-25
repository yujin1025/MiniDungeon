// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacter.h"
#include "../AI/MDAIController.h"
#include <MDNetworkManager.h>
#include <Kismet/GameplayStatics.h>
#include <Game/MDPlayerController.h>
#include "Component/AttackComponent.h"
#include "Components/WidgetComponent.h"
#include "../Widget/HPBarWidget.h"
#include "PlayableCharacter.h"
#include "Component/HealthComponent.h"

ANonPlayableCharacter::ANonPlayableCharacter()
{
	AIControllerClass = AMDAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//AutoPossessPlayer = EAutoReceiveInput::Disabled; //빙의하지 않음

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
}

bool ANonPlayableCharacter::Attack(APlayableCharacter* Player, float hp)
{
	if (IsDead)
		return false;

	if (IsValid(Player))
	{
		AttackedObjectCurrentHp.Empty();
		AttackedObjectCurrentHp.Add(Player->GetObjectID(), hp);
	}

	//TODO : 공격 타입에 따라 애니메이션 실행
	ActionComponentMap[EAttackType::QSkillAttack]->PlayAttackMontage();

	CurrentActionCoolTimeMap[EAttackType::QSkillAttack] = CurrentDeltaTime + ActionCoolTimeMap[EAttackType::QSkillAttack];

	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager->isHost)
	{
		Protocol::CTS_MONSTER_ATTACK AttackPkt;

		// 패킷을 SendBufferRef로 직렬화
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(AttackPkt);
		if (IsValid(networkManager))
		{
			networkManager->SendPacket(sendBuffer);
		}
	}

	return true;
}

void ANonPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();

	FVector Location = GetActorLocation();
	DestInfo->set_object_id(ObjectID);
	DestInfo->set_x(Location.X);
	DestInfo->set_y(Location.Y);
	DestInfo->set_z(Location.Z);
	DestInfo->set_yaw(GetControlRotation().Yaw);

	SetMoveState(Protocol::MOVE_STATE_IDLE);
	TargetLocation = GetActorLocation();

	if (HPBarWidget && HPBarWidget->GetWidget() != nullptr)
	{
		UHPBarWidget* HPWidget = Cast<UHPBarWidget>(HPBarWidget->GetWidget());
		if (HPWidget)
		{
			HPWidget->SetHealthComponent(HealthComponent);
		}
	}
}

void ANonPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager->isHost)
	{
		// State 정보
		if (GetVelocity() == FVector::Zero())
			SetMoveState(Protocol::MOVE_STATE_IDLE);
		else
			SetMoveState(Protocol::MOVE_STATE_RUN);

		MovePacketSendTimer -= DeltaTime;

		if (MovePacketSendTimer <= 0)
		{
			// 이동 패킷 전송
			Protocol::CTS_MOVE MovePkt;
			Protocol::PosInfo* Info = new Protocol::PosInfo();
			Info->CopyFrom(*PosInfo);
			Info->set_state(GetMoveState());
			MovePkt.set_allocated_info(Info);

			// 패킷을 SendBufferRef로 직렬화
			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
			if (IsValid(networkManager)) 
			{
				networkManager->SendPacket(sendBuffer);
			}
		}
	}
}
