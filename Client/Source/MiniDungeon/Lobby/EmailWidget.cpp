// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/EmailWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <MDNetworkManager.h>

UEmailWidget::UEmailWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEmailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(IsValid(SendButton))
	{
		SendButton->OnClicked.AddDynamic(this, &UEmailWidget::OnSendButtonClicked);
	}
}


void UEmailWidget::OnSendButtonClicked()
{
	if(IsValid(EmailInput))
	{
		SendEmailAddress = EmailInput->GetText().ToString();
		
		Protocol::CTS_EMAIL_VERIFICATION pkt;
		pkt.set_email(TCHAR_TO_UTF8(*SendEmailAddress));

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		networkManager->SendPacket(sendBuffer);
	}
}
