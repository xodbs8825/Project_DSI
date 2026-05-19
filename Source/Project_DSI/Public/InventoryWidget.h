// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

struct FInventoryItem;

class UPanelWidget;
class UButton;
class UTextBlock;

class UItemTypeTabWidget;
class UInventoryDetailWidget;
class UItemSlotWidget;
class UInventoryItemPromptWidget;

enum class EItemSlotFocusState : uint8
{
	None,
	Hovered,
	Selected
};

UCLASS()
class PROJECT_DSI_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitInventoryWidget();

	void SetInventoryItems(const TArray<FInventoryItem> InventoryItems);

	UFUNCTION(BlueprintCallable)
	void LoadInventory(class UInventoryManager* Inventory, EItemType ItemType);

	void ShowPrompt(UItemSlotWidget* ClickedSlot);

	UFUNCTION()
	void HandleHovered(UItemSlotWidget* HoveredSlot);

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandleClicked(UItemSlotWidget* ClickedSlot);

	UFUNCTION()
	void ItemTypeChanged(EItemType ItemType);

public:
	UPROPERTY()
	int32 ColumnCount = 9;

	UPROPERTY()
	int32 CurrentRowCount = 4;

	UPROPERTY(meta = (BindWidget))
	UItemTypeTabWidget* ConsumableTab;

	UPROPERTY(meta = (BindWidget))
	UItemTypeTabWidget* KeyItemTab;

	UPROPERTY(meta = (BindWidget))
	UItemTypeTabWidget* UpgradeMaterialTab;

	UPROPERTY(meta = (BindWidget))
	UInventoryDetailWidget* DetailWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UPanelWidget* ItemListPanel;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryItemPromptWidget> PromptWidgetClass;
	UInventoryItemPromptWidget* PromptWidget;

	EItemType CurrentItemType;

	TMap<EItemType, UItemTypeTabWidget*> ItemTypeMap;

	EItemSlotFocusState FocusState = EItemSlotFocusState::None;
	FInventoryItem* FocusedItem;
	UItemSlotWidget* SelectedSlot;
};
