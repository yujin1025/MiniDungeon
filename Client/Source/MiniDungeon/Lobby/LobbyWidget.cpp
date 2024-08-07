// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(IsValid(LOGINButton))
	{
		LOGINButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnLOGINButtonClicked);
	}

}

void ULobbyWidget::OnLOGINButtonClicked()
{
	if(IsValid(IDInput) && IsValid(PWInput))
	{
		FString ID = IDInput->GetText().ToString();
		FString PW = PWInput->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("ID : %s, PW : %s"), *ID, *PW);
		{
			Protocol::CTS_LOGIN pkt;
			pkt.set_id(TCHAR_TO_UTF8(*ID));
			pkt.set_pw(TCHAR_TO_UTF8(*PW));

			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			networkManager->SendPacket(sendBuffer);
		}
	}
}
