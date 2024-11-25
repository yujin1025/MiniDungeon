// Fill out your copyright notice in the Description page of Project Settings.


#include "Drongo.h"
#include "NonPlayableCharacter.h"
#include "../Component/HealthComponent.h"

void ADrongo::OnUseQSkill()
{
    Fire(-10.0f);
}

void ADrongo::OnUseESkill()
{
    Fire(-15.0f);
}

void ADrongo::OnUseShiftSkill()
{
    Fire(-20.0f);
}

void ADrongo::Fire(float DamageAmount)
{
    FVector CameraLocation;
    FRotator CameraRotation;
    Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector Start = CameraLocation;
    FVector End = Start + (CameraRotation.Vector() * 3500.0f); //길이 조절하기

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, CollisionParams))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            ANonPlayableCharacter* DamagedCharacter = Cast<ANonPlayableCharacter>(HitActor);
            if (DamagedCharacter)
            {
                UHealthComponent* HealthComp = DamagedCharacter->FindComponentByClass<UHealthComponent>();
                if (HealthComp)
                {
                    HealthComp->ChangeHealth(this, -DamageAmount);
                }
            }
        }
    }
}
