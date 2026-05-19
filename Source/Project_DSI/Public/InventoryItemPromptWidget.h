// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemPromptWidget.generated.h"

struct FInventoryItem;
struct FItemData;

class UButton;
class UPanelWidget;
class UTextBlock;

enum class EItemPromptState : uint8
{
	None,
	Use,
	Drop,
	Discard
};

UCLASS()
class PROJECT_DSI_API UInventoryItemPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetPromptWidget(class UItemSlotWidget* ItemSlot);
	void ResetPromptWidget(bool PromptWidgetVisibility);

	UFUNCTION()
	void OnUseButtonClicked();

	UFUNCTION()
	void OnDropButtonClicked();

	UFUNCTION()
	void OnDiscardButtonClicked();

	UFUNCTION()
	void OnDropAllButtonClicked();

	UFUNCTION()
	void OnDiscardAllButtonClicked();

	UFUNCTION()
	void OnCountUpButtonClicked();

	UFUNCTION()
	void OnCountDownButtonClicked();

	UFUNCTION()
	void OnYesButtonClicked();

	UFUNCTION()
	void OnNoButtonClicked();

	UFUNCTION()
	void OnConfirmationYesButtonClicked();

	UFUNCTION()
	void OnConfirmationNoButtonClicked();

public:
	UPROPERTY(meta = (BindWidget))
	UButton* UseButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DropButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DiscardButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DropAllButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DiscardAllButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CountUpButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CountDownButton;

	UPROPERTY(meta = (BindWidget))
	UButton* YesButton;

	UPROPERTY(meta = (BindWidget))
	UButton* NoButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmationYesButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmationNoButton;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ItemOptionPanel;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ItemCountPanel;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ConfirmPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	UPROPERTY()
	class UInventoryWidget* InventoryWidget;

	FItemData* ItemData;
	FInventoryItem* Item;

	int32 CurrentItemCount = 1;
	int32 MaxItemCount;

	EItemPromptState ItemPromptState = EItemPromptState::None;
};
