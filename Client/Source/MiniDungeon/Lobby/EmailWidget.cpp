// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/EmailWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <MDNetworkManager.h>
#include <Kismet/GameplayStatics.h>
#include "LobbyPlayerController.h"
#include "Components/CircularThrobber.h"

UEmailWidget::UEmailWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEmailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(IsValid(LoadingCircle))
	{
		LoadingCircle->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &UEmailWidget::OnCancelButtonClicked);
	}

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

		if (IsValid(LoadingCircle))
		{
			LoadingCircle->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UEmailWidget::OnCancelButtonClicked()
{
	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(pc))
	{
		pc->CloseAllPopupWidget();
	}
}

void UEmailWidget::CloseEmailWidget()
{
	if(IsValid(EmailInput))
	{
		EmailInput->SetText(FText::FromString(""));
	}

	if (IsValid(LoadingCircle))
	{
		LoadingCircle->SetVisibility(ESlateVisibility::Hidden);
	}

	SendEmailAddress = "";
	SetVisibility(ESlateVisibility::Hidden);
}
