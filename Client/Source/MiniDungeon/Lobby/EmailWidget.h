// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmailWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UEmailWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UEmailWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> EmailInput;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> SendButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCircularThrobber> LoadingCircle;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Email", meta = (AllowPrivateAccess = "true"))
	FString SendEmailAddress;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULoginWidget> ParentWidget;
public:
	UFUNCTION()
	FString GetSendEmailAddr() { return SendEmailAddress; }

	UFUNCTION()
	void SetParentWidget(ULoginWidget* parent) { ParentWidget = parent; }
private:
	UFUNCTION()
	void OnSendButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

public:
	UFUNCTION()
	void CloseEmailWidget();
};
