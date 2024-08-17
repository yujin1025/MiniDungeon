// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
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
