// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuthWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UAuthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UAuthWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> AuthInput;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> AuthButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Email", meta = (AllowPrivateAccess = "true"))
	FString EmailAddress;

public:
	UFUNCTION()
	FString GetEmailAddr() { return EmailAddress; }

	UFUNCTION()
	void SetEmailAddr(FString email) { EmailAddress = email; }
private:
	UFUNCTION()
	void OnAuthButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

public:
	UFUNCTION()
	void CloseAuthWidget();
};
