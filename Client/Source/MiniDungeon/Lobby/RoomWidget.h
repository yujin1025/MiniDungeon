// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidget.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;
class UButton;

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
	TObjectPtr<UButton> StartButton;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Player1_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Player3_Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Player4_Name;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Player1_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Player2_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Player3_Image;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Player4_Image;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Player1_Button;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Player2_Button;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Player3_Button;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Player4_Button;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "TextBlock")
	TArray<TObjectPtr<class UTextBlock>> PlayerNames;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CharacterImage")
	TArray<TObjectPtr< UImage>> PlayerCharacters;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Button")
	TArray<TObjectPtr<UButton>> PlayerButtons;

	UPROPERTY(EditAnywhere, Category = "PlayerInfo")
	TMap<uint32, uint64> IndexToPlayerID;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CharacterImage", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<class UMaterialInterface>> CharacterImages;

	UFUNCTION()
	void ChangeCharacterImage(uint32 index);

public:
	UFUNCTION()
	void OnClickedStartButton();

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<class URoomListViewItemData> RoomData;

	UFUNCTION()
	void OnClickedCharacterImage();

public:
	TObjectPtr<class URoomListViewItemData> GetRoomData() { return RoomData; }
	void SetRoomData(class URoomListViewItemData* data);
};
