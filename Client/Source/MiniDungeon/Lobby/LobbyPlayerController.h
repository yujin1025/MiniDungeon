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
	UPROPERTY()
	TSubclassOf<UUserWidget> RoomWidgetClass;

	UPROPERTY()
	TObjectPtr<class URoomWidget> RoomWidget;
private:
	Protocol::PlayerInfo* PlayerInfo;

public:
	const Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& info);
	void SetCharacter(const Protocol::PlayerType characterType) { PlayerInfo->set_player_type(characterType); }

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TMap<FString, TObjectPtr<class URoomListViewItemData>> RoomList;

public:
	class URoomListViewItemData* AddRoomData(const Protocol::RoomInfo& info);

	class URoomListViewItemData* UpdateRoomData(const Protocol::RoomInfo& info);

	TMap<FString, TObjectPtr<class URoomListViewItemData>>& GetRoomList() { return RoomList; }

public:
	void CreateRoom(const Protocol::RoomInfo& info, bool isHost);

	void OpenLobbyWidget();

	void HandleOpenLobbyWidget(const Protocol::STC_ENTER_LOBBY& enterLobbyPkt);

	void JoinRoom(const Protocol::RoomInfo& info);

	void ChangeCharacter(const Protocol::STC_CHANGE_CHARACTER& changeCharacterPkt);

	void LeaveRoom(const Protocol::STC_LEAVE_ROOM& leaveRoomPkt);
};
