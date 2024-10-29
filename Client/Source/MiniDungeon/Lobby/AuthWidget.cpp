// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/AuthWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <MDNetworkManager.h>
#include <Kismet/GameplayStatics.h>
#include "LobbyPlayerController.h"

UAuthWidget::UAuthWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UAuthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &UAuthWidget::OnCancelButtonClicked);
	}

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

void UAuthWidget::OnCancelButtonClicked()
{
	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(pc))
	{
		pc->CloseAllPopupWidget();
	}
}

void UAuthWidget::CloseAuthWidget()
{
	if(IsValid(AuthInput))
	{
		AuthInput->SetText(FText::FromString(""));
	}
	EmailAddress = "";
	SetVisibility(ESlateVisibility::Hidden);
}
