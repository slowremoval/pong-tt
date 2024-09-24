// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UEditableText;
class UButton;

UCLASS()
class PONG_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> IPTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConnectButton;

	UFUNCTION()
	void OnConnectButtonPressed();

	UFUNCTION()
	void OnIPTextBoxChanged(const FText& Text);

	bool IsValidIPAddressCharacter(TCHAR Character) const;
};