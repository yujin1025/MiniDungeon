// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomListViewItem.h"
#include "RoomListViewItemData.h"
#include "Components/TextBlock.h"

void URoomListViewItem::NativeConstruct()
{
	Super::NativeConstruct();

	RoomNameTextBox = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomNameTextBox")));
	PlayerNumTextBox = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerNumTextBox")));

	if (IsValid(RoomNameTextBox))
	{
		RoomNameTextBox->SetText(FText::FromString(TEXT("")));
	}
	
}

void URoomListViewItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	RoomData = Cast<URoomListViewItemData>(ListItemObject);
	RoomNameTextBox->SetText(FText::FromString(RoomData->RoomName));

	FString formattedString = FString::Printf(TEXT("%d / 4"), RoomData->PlayerNum);
	FText playernum = FText::FromString(formattedString);

	PlayerNumTextBox->SetText(playernum);
}
