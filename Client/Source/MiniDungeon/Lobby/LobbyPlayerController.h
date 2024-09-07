// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Protocol.pb.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ALobbyPlayerController();
	~ALobbyPlayerController();

	UFUNCTION()
	void OpenLobbyWidget();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<class UUserWidget> LoginWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULoginWidget> LoginWidget;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULobbyWidget> LobbyWidget;
private:
	Protocol::PlayerInfo* PlayerInfo;

public:
	Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& info);

public:
	void CreateRoom(uint64 roomIndex, const FString& roomName, const FString& password, const Protocol::PlayerInfo& info);
	void CreateRoom(const Protocol::RoomInfo& info);
};
