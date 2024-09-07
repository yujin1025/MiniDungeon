// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Protocol.pb.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	ULobbyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UListView> RoomListView;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> RoomNameInput;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PasswordInput;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CreateButton;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> JoinButton;

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSet<TObjectPtr<class URoomListViewItemData>> RoomList;

public:
	TObjectPtr<class URoomListViewItemData> AddRoomData(const Protocol::RoomInfo& info);

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> RoomWidgetClass;

	UPROPERTY()
	TObjectPtr<class URoomWidget> RoomWidget;

public:	
	void CreateRoom(const Protocol::RoomInfo& info, bool isHost);

	UFUNCTION()
	void RemoveRoom(const uint64 roomIndex);

	UFUNCTION()
	void UpdateRoom(const uint64 roomIndex, const uint64 playerNum);

	UFUNCTION()
	void ClearRoomList();
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TObjectPtr<class ALobbyPlayerController> Owner;
public:
	UFUNCTION()
	void RefreshListView();

	UFUNCTION()
	void OnClickedCreateButton();

	UFUNCTION()
	void OnClickedJoinButton();
};
