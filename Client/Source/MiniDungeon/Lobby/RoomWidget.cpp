// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Lobby/RoomListViewItemData.h"

URoomWidget::URoomWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FString AuroraImagePath = FString::Printf(TEXT("/Script/Engine.Material'/Game/Assets/UI/Lobby/MT_TR2D_Aurora.MT_TR2D_Aurora'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> AuroraImage(*AuroraImagePath);
	if (AuroraImage.Succeeded())
	{
		CharacterImages.AddUnique(AuroraImage.Object);
	}

	FString DrongoImagePath = FString::Printf(TEXT("/Script/Engine.Material'/Game/Assets/UI/Lobby/MT_TR2D_Drongo.MT_TR2D_Drongo'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> DrongoImage(*DrongoImagePath);
	if (DrongoImage.Succeeded())
	{
		CharacterImages.AddUnique(DrongoImage.Object);
	}

	//PlayerNames.AddUnique(Player1_Name);
	//PlayerNames.AddUnique(Player2_Name);
	//PlayerNames.AddUnique(Player3_Name);
	//PlayerNames.AddUnique(Player4_Name);

	//PlayerCharacters.AddUnique(Player1_Image);
	//PlayerCharacters.AddUnique(Player2_Image);
	//PlayerCharacters.AddUnique(Player3_Image);
	//PlayerCharacters.AddUnique(Player4_Image);
}

void URoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(StartButton))
	{
		StartButton->OnClicked.AddDynamic(this, &URoomWidget::OnClickedStartButton);
	}

	{
		if (PlayerNames.Num() != 0)
			PlayerNames.Empty();
		
		for (int i = 1; i <= 4; i++)
		{
			FString PlayerNum_Name = FString::Printf(TEXT("Player%d_Name"), i);
			PlayerNames.Add(Cast<UTextBlock>(GetWidgetFromName(FName(PlayerNum_Name))));
		}

		int index = 0;
		for (auto& player : RoomData->GetPlayers())
		{
			FText playerName = FText::FromString(UTF8_TO_TCHAR(player.Value->player_name().c_str()) + FString::FromInt(player.Value->player_id()));
			PlayerNames[index]->SetText(playerName);
			index++;
		}
	}

	{
		if (PlayerCharacters.Num() != 0)
			PlayerCharacters.Empty();

		for (int i = 1; i <= 4; i++)
		{
			FString PlayerNum_Image = FString::Printf(TEXT("Player%d_Image"), i);
			PlayerCharacters.Add(Cast<UImage>(GetWidgetFromName(FName(PlayerNum_Image))));
		}

		int index = 0;
		for (auto& player : RoomData->GetPlayers())
		{
			switch (player.Value->player_type())
			{
				case Protocol::PLAYER_TYPE_AURORA:
					PlayerCharacters[index]->SetBrushFromMaterial(CharacterImages[0]);
					PlayerCharacters[index]->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
					index++;
					break;
				case Protocol::PLAYER_TYPE_DRONGO:
					PlayerCharacters[index]->SetBrushFromMaterial(CharacterImages[1]);
					PlayerCharacters[index]->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
					index++;
					break;
			}
		}
	}

}

void URoomWidget::OnClickedStartButton()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked"));
}

void URoomWidget::SetRoomData(URoomListViewItemData* data)
{
	RoomData = data;
}
