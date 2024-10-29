// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/SignUpWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>
#include "Lobby/LobbyPlayerController.h"
#include "Components/CheckBox.h"

USignUpWidget::USignUpWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void USignUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(IDCheckBox))
	{
		IDCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	}

	if(IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &USignUpWidget::OnCancelButtonClicked);
	}

	if (IsValid(SIGNUPButton))
	{
		SIGNUPButton->OnClicked.AddDynamic(this, &USignUpWidget::OnSignUpButtonClicked);
	}

	if (IsValid(DuplicateButton))
	{
		DuplicateButton->OnClicked.AddDynamic(this, &USignUpWidget::OnDuplicateButtonClicked);
	}
}

void USignUpWidget::OnSignUpButtonClicked()
{
	if (IsValid(IDCheckBox))
	{
		if (IDCheckBox->CheckedState != ECheckBoxState::Checked)
		{
			return;
		}
	}
	
	if (IsValid(IDInput) && IsValid(PWInput))
	{
		FString ID = IDInput->GetText().ToString();
		FString PW = PWInput->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("ID : %s, PW : %s"), *ID, *PW);
		{
			Protocol::CTS_REGISTER pkt;
			pkt.set_id(TCHAR_TO_UTF8(*ID));
			pkt.set_pw(TCHAR_TO_UTF8(*PW));
			pkt.set_email(TCHAR_TO_UTF8(*EmailAddress));

			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			networkManager->SendPacket(sendBuffer);
		}
	}
}

void USignUpWidget::OnCancelButtonClicked()
{
	auto* pc = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(pc))
	{
		pc->CloseAllPopupWidget();
	}
}

void USignUpWidget::OnDuplicateButtonClicked()
{
	if (IsValid(IDInput))
	{
		FString ID = IDInput->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("ID : %s"), *ID);
		{
			Protocol::CTS_CHECK_DUPLICATE pkt;
			pkt.set_id(TCHAR_TO_UTF8(*ID));

			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			networkManager->SendPacket(sendBuffer);
		}
	}
}

void USignUpWidget::CloseSignUpWidget()
{
	if (IsValid(IDCheckBox))
	{
		IDCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	if(IsValid(IDInput))
	{
		IDInput->SetText(FText::FromString(""));
	}
	if(IsValid(PWInput))
	{
		PWInput->SetText(FText::FromString(""));
	}
	EmailAddress = "";

	SetVisibility(ESlateVisibility::Hidden);
}

void USignUpWidget::OnIDChecked(bool isDuplicated)
{
	if (IsValid(IDCheckBox))
	{
		if (isDuplicated == false)
		{
			IDCheckBox->SetCheckedState(ECheckBoxState::Checked);
		}
		else
		{
			IDCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
		}
	}
}
