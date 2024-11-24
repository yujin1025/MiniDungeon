// Fill out your copyright notice in the Description page of Project Settings.


#include "MDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/AttackComponent.h"
#include "../Component/HealthComponent.h"
#include "../Component/HitDeadComponent.h"
#include "../Game/MDGameMode.h"
#include "../Network/MDNetworkManager.h"
#include "../Game/MDGameInstance.h"
#include <Kismet/GameplayStatics.h>

AMDCharacter::AMDCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HitDeadComponent = CreateDefaultSubobject<UHitDeadComponent>(TEXT("HitDeadComponent"));

	ActionComponentMap.Empty();
	for (int i = 0; i < (int)EAttackType::Max; i++)
	{
		auto AttackType = (EAttackType)i;

		FString AttackTypeStr = GetEnumNameAsString(AttackType);
		FString ComponentName = TEXT("AttackComponent [") + AttackTypeStr + "]";
		auto Component = CreateDefaultSubobject<UAttackComponent>((FName)*ComponentName);
		ActionComponentMap.Add(AttackType, Component);

		ActionCoolTimeMap.Add(AttackType, 5.0f);
		CurrentActionCoolTimeMap.Add(AttackType, 0.0f);
	}


	PosInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();
}

void AMDCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	delete PosInfo;
	PosInfo = nullptr;

	delete DestInfo;
	DestInfo = nullptr;
}

FString AMDCharacter::GetEnumNameAsString(EAttackType EnumValue)
{
	switch (EnumValue)
	{
	case EAttackType::QSkillAttack:
		return "QSkillAttack";

	case EAttackType::ESkillAttack:
		return "ESkillAttack";

	case EAttackType::ShiftAttack:
		return "ShiftAttack";
	}

	return "";
}

void AMDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentDeltaTime += DeltaTime;
}

void AMDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMDCharacter::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Super Begin"));
	Super::BeginPlay();

	switch (CharacterType)
	{
	case ECharacterType::Aurora:
		CharacterId = 0;
		break;
	case ECharacterType::Drongo:
		CharacterId = 1;
		break;
	case ECharacterType::Khaimera:
		CharacterId = 2;
		break;
	case ECharacterType::Grux:
		CharacterId = 3;
		break;
	}
	MD_LOG(LogMDNetwork, Log, TEXT("Super End"));
}

void AMDCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void AMDCharacter::Move(const FVector2D Value)
{
	if (IsDead)
		return;

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void AMDCharacter::Look(const FVector2D Value)
{
	if (IsDead)
		return;

	if (Controller != nullptr)
	{
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(Value.Y);
	}
}

void AMDCharacter::SendAttackPacket(EAttackType AttackType)
{
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if(networkManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetworkManager is nullptr in SendAttackPacket()"));
		return;
	}

	float damage = 0.0f;
	switch (AttackType)
	{
	case EAttackType::QSkillAttack:
		damage = 10.0f; 
		break;
	case EAttackType::ESkillAttack:
		damage = 20.0f; 
		break;
	case EAttackType::ShiftAttack:
		damage = 30.0f; 
		break;
	}

	Protocol::AttackInfo attackInfo;
	if (networkManager->PlayerInfos.Contains(networkManager->PlayerID))
	{
		attackInfo.set_attack_object_id(ObjectID);
		attackInfo.set_player_type(networkManager->PlayerInfos[networkManager->PlayerID]->player_type());

		attackInfo.set_damage(damage); // 데미지 설정
		attackInfo.set_attack_type(static_cast<uint64>(AttackType)); // 공격 타입 설정

		// 패킷을 생성
		Protocol::CTS_ATTACK attackPkt;
		*attackPkt.mutable_info() = attackInfo;

		// SendBufferRef로 직렬화
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(attackPkt);

		// 네트워크 매니저를 통해 패킷 전송
		networkManager->SendPacket(sendBuffer);
	}
}

bool AMDCharacter::UseSkill(EAttackType AttackType)
{
	if (IsDead)
		return false;

	if (IsSatisfiedAttack(AttackType) == false)
		return false;

	if (ProgressingAttackType != EAttackType::Max)
		return false;

	if (ActionComponentMap.Contains(AttackType) == false)
		return false;

	if (ActionCoolTimeMap.Contains(AttackType) == false)
		return false;

	ActionComponentMap[AttackType]->PlayAttackMontage();
	CurrentActionCoolTimeMap[AttackType] = CurrentDeltaTime + ActionCoolTimeMap[AttackType];

	SendAttackPacket(AttackType);

	switch (AttackType)
	{
	case EAttackType::QSkillAttack:
		OnUseQSkill();
		break;
	case EAttackType::ESkillAttack:
		OnUseESkill();
		break;
	case EAttackType::ShiftAttack:
		OnUseShiftSkill();
		break;
	}

	//OnAttackEnd.Broadcast();
	ProgressingAttackType = AttackType;
	return true;
}

void AMDCharacter::OnFinishedSkillMotion(EAttackType AttackType)
{
	ProgressingAttackType = EAttackType::Max;
	if (AttackType != EAttackType::Max)
	{
		OnUseSkillDelegate.Broadcast(AttackType);
	}
}

bool AMDCharacter::IsSatisfiedAttack(EAttackType AttackType)
{
	return !CheckCoolTime(AttackType) && !IsDead;
}

bool AMDCharacter::CheckCoolTime(EAttackType AttackType)
{
	if (ActionCoolTimeMap.Contains(AttackType) == false)
		return true;

	if (CurrentActionCoolTimeMap.Contains(AttackType) == false)
		return true;

	float CurrentCoolTime = CurrentActionCoolTimeMap[AttackType];
	return CurrentCoolTime > CurrentDeltaTime;
}

void AMDCharacter::OnHit()
{
	if (IsDead)
		return;

	if (HitDeadComponent)
	{
		HitDeadComponent->PlayHitMontage();
	}

	OnFinishedSkillMotion(ProgressingAttackType);
}

void AMDCharacter::OnDie()
{
	if (IsDead)
		return;

	IsDead = true;

	if (HitDeadComponent)
	{
		HitDeadComponent->PlayDeadMontage();
	}
}

bool AMDCharacter::IsPlayer()
{
	if (Controller)
	{
		return Controller->IsPlayerController();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is nullptr in IsPlayer()"));
		return false;
	}
}

FVector AMDCharacter::GetLookVector(const AMDCharacter* Target) const
{
	return Target->GetActorLocation() - GetActorLocation();
}

FVector AMDCharacter::GetTargetPosition(ECollisionChannel Channel, float RayCastDistance, OUT bool& IsFoundTarget)
{
	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Start = CameraLocation;
	FVector End = Start + (CameraRotation.Vector() * RayCastDistance);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Channel, CollisionParams))
	{
		IsFoundTarget = true;
		return HitResult.Location;
	}

	IsFoundTarget = false;
	return FVector::ZeroVector;
}

void AMDCharacter::RotateToTarget(const AMDCharacter* Target, float RotationSpeed)
{
	if (IsValid(Target))
		return;

	FVector LookVector = GetLookVector(Target);
	LookVector.Z = 0.f;

	FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
	SetRotation(TargetRotation, RotationSpeed);
}

void AMDCharacter::SetRotation(FRotator Rotation, float RotationSpeed)
{
	FRotator TargetRotation = FMath::RInterpTo(GetActorRotation(), Rotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	SetActorRotation(TargetRotation);
}

void AMDCharacter::SetMoveState(Protocol::MoveState State)
{
	if (PosInfo->state() == State)
		return;

	PosInfo->set_state(State);

	// TODO
}

void AMDCharacter::SetPosInfo(const Protocol::PosInfo& Info)
{
	if (PosInfo->object_id() != 0)
	{
		assert(PosInfo->object_id() == Info.object_id());
	}

	PosInfo->CopyFrom(Info);

	FVector Location(Info.x(), Info.y(), Info.z());
	SetActorLocation(Location);

	TargetLocation = Location;
}

void AMDCharacter::SetDestInfo(const Protocol::PosInfo& Info)
{
	if (PosInfo->object_id() != 0)
	{
		assert(PosInfo->object_id() == Info.object_id());
	}

	// Dest에 최종 상태 복사.
	DestInfo->CopyFrom(Info);

	// 상태만 바로 적용하자.
	SetMoveState(Info.state());
	TargetLocation = FVector(Info.x(), Info.y(), Info.z());
}



