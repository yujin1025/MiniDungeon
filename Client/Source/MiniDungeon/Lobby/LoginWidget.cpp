// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(LOGINButton))
	{
		LOGINButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLOGINButtonClicked);
	}
}

void ULoginWidget::OnLOGINButtonClicked()
{
	if (IsValid(IDInput) && IsValid(PWInput))
	{
		FString ID = IDInput->GetText().ToString();
		FString PW = PWInput->GetText().ToString();
		uint64 PlayerID = FCString::Atoi64(*PlayerIDInput->GetText().ToString());
		UE_LOG(LogTemp, Warning, TEXT("ID : %s, PW : %s"), *ID, *PW);
		{
			Protocol::CTS_LOGIN pkt;
			pkt.set_id(TCHAR_TO_UTF8(*ID));
			pkt.set_pw(TCHAR_TO_UTF8(*PW));
			pkt.set_player_id(PlayerID);

			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			networkManager->SendPacket(sendBuffer);
		}
	}
}
