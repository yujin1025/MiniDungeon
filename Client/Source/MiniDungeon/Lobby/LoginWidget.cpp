// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>
#include "EmailWidget.h"
#include "AuthWidget.h"
#include "SignUpWidget.h"

ULoginWidget::ULoginWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(LOGINButton))
	{
		LOGINButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLOGINButtonClicked);
	}

	if (IsValid(SIGNUPButton))
	{
		SIGNUPButton->OnClicked.AddDynamic(this, &ULoginWidget::OnSIGNUPButtonClicked);
	}

	if (IsValid(WBP_EmailWidget))
	{
		WBP_EmailWidget->SetVisibility(ESlateVisibility::Hidden);
		WBP_EmailWidget->SetParentWidget(this);
	}

	if (IsValid(WBP_AuthWidget))
	{
		WBP_AuthWidget->SetVisibility(ESlateVisibility::Hidden);
		WBP_AuthWidget->SetParentWidget(this);
	}

	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->SetVisibility(ESlateVisibility::Hidden);
		WBP_SignUpWidget->SetParentWidget(this);
	}
}

void ULoginWidget::OnLOGINButtonClicked()
{
	if (IsValid(IDInput) && IsValid(PWInput))
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

void ULoginWidget::OnSIGNUPButtonClicked()
{
	OpenEmailWidget();
}

void ULoginWidget::OpenEmailWidget()
{
	if (IsValid(WBP_AuthWidget))
	{
		WBP_AuthWidget->CloseAuthWidget();
	}

	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->CloseSignUpWidget();
	}

	if (IsValid(WBP_EmailWidget))
	{
		WBP_EmailWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULoginWidget::OepnAuthWidget()
{
	if (IsValid(WBP_EmailWidget))
	{
		WBP_EmailWidget->CloseEmailWidget();
	}

	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->CloseSignUpWidget();
	}

	if (IsValid(WBP_AuthWidget))
	{
		WBP_AuthWidget->SetEmailAddr(WBP_EmailWidget->GetSendEmailAddr());
		WBP_AuthWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULoginWidget::OpenSignUpWidget()
{
	if (IsValid(WBP_EmailWidget))
	{
		WBP_EmailWidget->CloseEmailWidget();
	}

	if (IsValid(WBP_AuthWidget))
	{
		WBP_AuthWidget->CloseAuthWidget();
	}

	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->SetEmailAddr(WBP_EmailWidget->GetSendEmailAddr());
		WBP_SignUpWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULoginWidget::OnRegistered()
{
	CloseAllPopupWidget();
}

void ULoginWidget::CloseAllPopupWidget()
{
	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->CloseSignUpWidget();
	}

	if (IsValid(WBP_EmailWidget))
	{
		WBP_EmailWidget->CloseEmailWidget();
	}

	if (IsValid(WBP_AuthWidget))
	{
		WBP_AuthWidget->CloseAuthWidget();
	}
}

void ULoginWidget::OnIDChecked(bool isDuplicated)
{
	if (IsValid(WBP_SignUpWidget))
	{
		WBP_SignUpWidget->OnIDChecked(isDuplicated);
	}
}
