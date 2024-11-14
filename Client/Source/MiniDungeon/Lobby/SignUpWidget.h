// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API USignUpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USignUpWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCheckBox> IDCheckBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> DuplicateButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> SIGNUPButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> IDInput;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PWInput;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Email", meta = (AllowPrivateAccess = "true"))
	FString EmailAddress;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULoginWidget> ParentWidget;
public:
	UFUNCTION()
	FString GetEmailAddr() { return EmailAddress; }

	UFUNCTION()
	void SetEmailAddr(FString email) { EmailAddress = email; }

	UFUNCTION()
	void SetParentWidget(ULoginWidget* parent) { ParentWidget = parent; }

protected:
	UFUNCTION()
	void OnSignUpButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

	UFUNCTION()
	void OnDuplicateButtonClicked();

public:
	UFUNCTION()
	void CloseSignUpWidget();

	UFUNCTION()
	void OnIDChecked(bool isDuplicated);
};
