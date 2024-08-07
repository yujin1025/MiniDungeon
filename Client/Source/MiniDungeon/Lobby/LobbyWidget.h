// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> LOGINButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> IDInput;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> PWInput;

	UFUNCTION()
	void OnLOGINButtonClicked();
};
