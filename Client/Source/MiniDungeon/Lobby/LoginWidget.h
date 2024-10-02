// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ULoginWidget : public UUserWidget
{

	GENERATED_BODY()
public:
	ULoginWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> LOGINButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> SIGNUPButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> IDInput;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PWInput;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PlayerIDInput;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class USignUpWidget> WBP_SignUpWidget;

protected:
	UFUNCTION()
	void OnLOGINButtonClicked();
	UFUNCTION()
	void OnSIGNUPButtonClicked();
};
