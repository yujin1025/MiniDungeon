// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/AuthWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <MDNetworkManager.h>

UAuthWidget::UAuthWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UAuthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(AuthButton))
	{
		AuthButton->OnClicked.AddDynamic(this, &UAuthWidget::OnAuthButtonClicked);
	}
}

void UAuthWidget::OnAuthButtonClicked()
{
	if(IsValid(AuthInput))
	{
		FString Auth = AuthInput->GetText().ToString();
		
		Protocol::CTS_AUTH pkt;
		pkt.set_email(TCHAR_TO_UTF8(*EmailAddress));
		pkt.set_auth_code(TCHAR_TO_UTF8(*Auth));

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
		networkManager->SendPacket(sendBuffer);
	}
}
