// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DSI_CharacterIndexSave.h"
#include "CharacterSaveSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UCharacterSaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetSlotDetail(struct FCharacterSlotInfo Info, class UTitleWidget* Owner);

	UFUNCTION()
	void OnSlotButtonClicked();

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CharacterNameText;

protected:
	struct FCharacterSlotInfo SlotInfo;

	UTitleWidget* TitleWidget;
};
