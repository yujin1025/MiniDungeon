// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RoomListViewItem.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API URoomListViewItem : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void NativeConstruct();

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> RoomNameTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNumTextBox;
};
