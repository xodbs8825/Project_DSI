// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void FadeOutScreen();

protected:
	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

	UFUNCTION()
	void OnCharacterNameChanged(const FText& Text);

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LoadGameButton;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* CharacterCreatePrompt;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* CharacterNameEditText;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* CharacterSlotPanel;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* SaveSlotPanel;

	UPROPERTY(meta = (BindWidget))
	class UFadeWidget* FadeWidget;

public:
	UPROPERTY(EditDefaultsOnly, Category = "SaveSlots")
	TSubclassOf<class UCharacterSaveSlotWidget> CharacterSaveSlotClass;
};
