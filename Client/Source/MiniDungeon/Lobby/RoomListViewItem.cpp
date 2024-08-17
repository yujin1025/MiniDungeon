// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomListViewItem.h"
#include "RoomListViewItemData.h"
#include "Components/TextBlock.h"

void URoomListViewItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoomListViewItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	auto roomListItem = Cast<URoomListViewItemData>(ListItemObject);
	RoomNameTextBox->SetText(roomListItem->RoomName);

	FString formattedString = FString::Printf(TEXT("%d / 4"), roomListItem->PlayerNum);
	FText playernum = FText::FromString(formattedString);

	PlayerNumTextBox->SetText(playernum);
}
