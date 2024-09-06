// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API URoomWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URoomWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> StartButton;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Player1_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Player2_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Player3_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Player4_Name;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Player1_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Player2_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Player3_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> Player4_Image;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextBlock", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<class UTextBlock>> PlayerNames;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CharacterImage", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<class UImage>> PlayerCharacters;

public:
	UFUNCTION()
	void OnClickedStartButton();

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<class URoomListViewItemData> RoomData;

public:
	TObjectPtr<class URoomListViewItemData> GetRoomData() { return RoomData; }
	void SetRoomData(class URoomListViewItemData* data);
};
