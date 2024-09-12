// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/RoomWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Lobby/RoomListViewItemData.h"
#include "LobbyPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "MDNetworkManager.h"

URoomWidget::URoomWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FString AuroraImagePath = FString::Printf(TEXT("/Script/Engine.Material'/Game/Assets/UI/Lobby/MT_TR2D_Aurora.MT_TR2D_Aurora'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> AuroraImage(*AuroraImagePath);
	if (AuroraImage.Succeeded())
	{
		CharacterImages.Add(Protocol::PLAYER_TYPE_AURORA, AuroraImage.Object);
	}

	FString DrongoImagePath = FString::Printf(TEXT("/Script/Engine.Material'/Game/Assets/UI/Lobby/MT_TR2D_Drongo.MT_TR2D_Drongo'"));
	ConstructorHelpers::FObjectFinder<UMaterialInterface> DrongoImage(*DrongoImagePath);
	if (DrongoImage.Succeeded())
	{
		CharacterImages.Add(Protocol::PLAYER_TYPE_DRONGO, DrongoImage.Object);
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

	if (IsValid(QuitButton))
	{
		QuitButton->OnClicked.AddDynamic(this, &URoomWidget::OnClickedQuitButton);
	}

	{
		if (PlayerNames.Num() != 0)
			PlayerNames.Empty();
		
		for (int i = 1; i <= 4; i++)
		{
			FString PlayerNum_Name = FString::Printf(TEXT("Player%d_Name"), i);
			PlayerNames.Add(Cast<UTextBlock>(GetWidgetFromName(FName(PlayerNum_Name))));
		}

		if (PlayerCharacters.Num() != 0)
			PlayerCharacters.Empty();

		for (int i = 1; i <= 4; i++)
		{
			FString PlayerNum_Image = FString::Printf(TEXT("Player%d_Image"), i);
			PlayerCharacters.Add(Cast<UImage>(GetWidgetFromName(FName(PlayerNum_Image))));
		}

		if (PlayerButtons.Num() != 0)
			PlayerButtons.Empty();

		for (int i = 1; i <= 4; i++)
		{
			FString PlayerNum_Button = FString::Printf(TEXT("Player%d_Button"), i);
			PlayerButtons.Add(Cast<UButton>(GetWidgetFromName(FName(PlayerNum_Button))));
		}
	}

	RefreshPlayers();
}


void URoomWidget::OnClickedStartButton()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked"));
}

void URoomWidget::OnClickedQuitButton()
{
	Protocol::CTS_LEAVE_ROOM pkt;
	pkt.set_roomindex(RoomData->RoomIndex);

	auto pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	const uint64 localPlayerIndex = pc->GetPlayerInfo()->player_id();
	pkt.set_player_id(localPlayerIndex);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void URoomWidget::HandleLeaveRoom()
{
	RefreshPlayers();
}

void URoomWidget::RefreshPlayers()
{
	for (auto& playerButton : PlayerButtons)
	{
		if (playerButton->OnClicked.IsBound())
		{
			playerButton->OnClicked.Clear();
		}
	}

	int index = 0;

	for (auto& player : RoomData->GetPlayers())
	{
		auto pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

		if (player.Value->player_id() == pc->GetPlayerInfo()->player_id())
		{
			PlayerButtons[index]->OnClicked.AddDynamic(this, &URoomWidget::OnClickedCharacterImage);
		}

		IndexToPlayerID.Add(index, player.Value->player_id());
		FText playerName = FText::FromString(UTF8_TO_TCHAR(player.Value->player_name().c_str()) + FString::FromInt(player.Value->player_id()));
		PlayerNames[index]->SetText(playerName);

		switch (player.Value->player_type())
		{
		case Protocol::PLAYER_TYPE_AURORA:
			PlayerCharacters[index]->SetBrushFromMaterial(CharacterImages[Protocol::PLAYER_TYPE_AURORA]);
			PlayerCharacters[index]->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			index++;
			break;
		case Protocol::PLAYER_TYPE_DRONGO:
			PlayerCharacters[index]->SetBrushFromMaterial(CharacterImages[Protocol::PLAYER_TYPE_DRONGO]);
			PlayerCharacters[index]->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			index++;
			break;
		}
	}

	for (index; index < 4; index++)
	{
		PlayerNames[index]->SetText(FText::FromString(TEXT("EMPTY")));
		PlayerCharacters[index]->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.f));
	}
}

void URoomWidget::OnClickedCharacterImage()
{
	auto pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	const uint64 localPlayerIndex = pc->GetPlayerInfo()->player_id();

	if (pc->GetPlayerInfo()->player_type() == Protocol::PLAYER_TYPE_AURORA)
	{
		ChangeCharacterImage(localPlayerIndex, Protocol::PLAYER_TYPE_DRONGO);
	}
	else
	{
		ChangeCharacterImage(localPlayerIndex, Protocol::PLAYER_TYPE_AURORA);
	}

	Protocol::CTS_CHANGE_CHARACTER pkt;
	pkt.set_roomindex(RoomData->RoomIndex);
	pkt.set_player_id(localPlayerIndex);
	pkt.set_character(RoomData->GetPlayers()[localPlayerIndex]->player_type());

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	networkManager->SendPacket(sendBuffer);
}

void URoomWidget::ChangeCharacterImage(uint32 playerIndex, Protocol::PlayerType type, bool isLocal)
{
	const auto roomCharacterIndex = IndexToPlayerID.FindKey(playerIndex);

	auto pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	switch (type)
	{
		case Protocol::PLAYER_TYPE_AURORA:
			PlayerCharacters[*roomCharacterIndex]->SetBrushFromMaterial(CharacterImages[Protocol::PLAYER_TYPE_AURORA]);
			if (isLocal)
			{
				pc->SetCharacter(Protocol::PLAYER_TYPE_AURORA);
			}
			RoomData->GetPlayers()[playerIndex]->set_player_type(Protocol::PLAYER_TYPE_AURORA);
			break;
		case Protocol::PLAYER_TYPE_DRONGO:
			PlayerCharacters[*roomCharacterIndex]->SetBrushFromMaterial(CharacterImages[Protocol::PLAYER_TYPE_DRONGO]);
			if (isLocal)
			{
				pc->SetCharacter(Protocol::PLAYER_TYPE_DRONGO);
			}
			RoomData->GetPlayers()[playerIndex]->set_player_type(Protocol::PLAYER_TYPE_DRONGO);
			break;
	}
}

void URoomWidget::SetRoomData(URoomListViewItemData* data)
{
	RoomData = data;
}
