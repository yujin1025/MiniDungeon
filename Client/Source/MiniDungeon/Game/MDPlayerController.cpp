// Fill out your copyright notice in the Description page of Project Settings.


#include "MDPlayerController.h"
#include "MDPlayerState.h"
#include "../Character/MDCharacter.h"
#include "../Game/MDGameState.h"
#include "../Game/MDGameMode.h"
#include "../Widget/MDWidget.h"
#include <Character/PlayableCharacter.h>

AMDPlayerController::AMDPlayerController()
{
    PlayerInfo = new Protocol::PlayerInfo();
}

void AMDPlayerController::BeginDestroy()
{
    Super::BeginDestroy();

    delete PlayerInfo;
    PlayerInfo = nullptr;
}

void AMDPlayerController::BeginPlay()
{
    MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
    Super::BeginPlay();
    MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AMDPlayerController::PostInitializeComponents()
{
    MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
    Super::PostInitializeComponents();
    MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AMDPlayerController::OnPossess(APawn* aPawn)
{
    MD_LOG(LogMDNetwork, Log, TEXT("%s Begin"), *aPawn->GetName());
    Super::OnPossess(aPawn);
    MD_LOG(LogMDNetwork, Log, TEXT("%s End"), *aPawn->GetName());
    
    if (IsValid(Cast<APlayableCharacter>(aPawn)))
    {
        SetInputMode(FInputModeGameOnly());
    }
}

void AMDPlayerController::CheckGameState()
{
    if (bGameStateShown)
        return;

    AMDGameState* GameState = GetWorld()->GetGameState<AMDGameState>();
    if (!GameState)
        return;

    AMDGameMode* GameMode = GetWorld()->GetAuthGameMode<AMDGameMode>();
    if (!GameMode)
        return;

    /*if (GameState->IsDeadGrux() || GameMode->IsDeadPlayers())
    {
        if (GameState->IsDeadGrux())
        {
            if (ClearWidgetClass)
            {
                GameClearWidget = CreateWidget<UMDWidget>(GetWorld(), ClearWidgetClass);
                if (GameClearWidget != nullptr)
                {
                    GameClearWidget->AddToViewport();
                }
            }
        }
        if (GameMode->IsDeadPlayers())
        {
            if (OverWidgetClass)
            {
                GameOverWidget = CreateWidget<UMDWidget>(GetWorld(), OverWidgetClass);
                if (GameOverWidget != nullptr)
                {
                    GameOverWidget->AddToViewport();
                }
            }
        }
        SetPause(true);
        bShowMouseCursor = true;
        bGameStateShown = true;
    }*/
}

void AMDPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //CheckGameState();
}

void AMDPlayerController::OnPossessCharacter(AMDCharacter* aCharacter)
{
    this->OwnerCharacter = aCharacter;
}

void AMDPlayerController::SetPlayerInfo(const Protocol::PlayerInfo& info)
{
    PlayerInfo->CopyFrom(info);
}

AMDPlayerState* AMDPlayerController::GetState()
{
    return Cast<AMDPlayerState>(PlayerState);
}

AMDCharacter* AMDPlayerController::GetCharacter()
{
    return OwnerCharacter;
}
