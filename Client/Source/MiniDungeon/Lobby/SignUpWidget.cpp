// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/SignUpWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>
#include <MDNetworkManager.h>

void USignUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SIGNUPButton->OnClicked.AddDynamic(this, &USignUpWidget::OnSignUpButtonClicked);
}

void USignUpWidget::OnSignUpButtonClicked()
{
	if (IsValid(IDInput) && IsValid(PWInput))
	{
		FString ID = IDInput->GetText().ToString();
		FString PW = PWInput->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("ID : %s, PW : %s"), *ID, *PW);
		{
			//Protocol::CTS_SIGNUP pkt;
			//pkt.set_id(TCHAR_TO_UTF8(*ID));
			//pkt.set_pw(TCHAR_TO_UTF8(*PW));

			//SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			//auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			//networkManager->SendPacket(sendBuffer);
		}
	}
	SetVisibility(ESlateVisibility::Hidden);
}
