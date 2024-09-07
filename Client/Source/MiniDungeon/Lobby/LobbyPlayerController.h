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

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSet<TObjectPtr<class URoomListViewItemData>> RoomList;

public:
	void CreateRoom(const Protocol::RoomInfo& info, bool isHost);

	void OpenLobbyWidget(const Protocol::STC_ENTER_LOBBY& enterLobbypkt);
};
