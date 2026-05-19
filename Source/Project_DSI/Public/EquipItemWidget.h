// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "EquipItemWidget.generated.h"

class UPanelWidget;
class UButton;
class UTextBlock;

class UEquipmentDetailWidget;
class UABCharacterStatComponent;

UCLASS()
class PROJECT_DSI_API UEquipItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitTabs();

	void RefreshList(UInventoryManager* Inventory, EEquipmentSlotType TargetSlotType);

	bool IsValidForSlot(EEquipmentSlotType SlotType, EItemType ItemType);
	EItemType GetItemType(EEquipmentSlotType SlotType);

	void CreateItemSlot(const FInventoryItem& Item, const FItemData& Data);

	void OnItemSelected(const FInventoryItem& Item);

	void OnTabSelected(EEquipmentSlotType Type);
	void SetIsTabFocused(bool bValue, UButton* TabButton, UTextBlock* TabText);

	UFUNCTION()
	void OnHelmsClicked();

	UFUNCTION()
	void OnBodyClicked();

	UFUNCTION()
	void OnGauntletsClicked();

	UFUNCTION()
	void OnPantsClicked();

	UFUNCTION()
	void OnBootsClicked();

	UFUNCTION()
	void OnHotkey_1Clicked();

	UFUNCTION()
	void OnHotkey_2Clicked();

	UFUNCTION()
	void OnHotkey_3Clicked();

	UFUNCTION()
	void OnHotkey_4Clicked();

	UFUNCTION()
	void OnHotkey_5Clicked();

	UFUNCTION()
	void OnHotkey_6Clicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<class UEquipItemSlot> EquipSlotWidgetClass;

public:
	UPROPERTY()
	int32 ColumnCount = 9;

	UPROPERTY()
	int32 CurrentRowCount = 4;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ItemListPanel;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ArmorTab;

	UPROPERTY(meta = (BindWidget))
	UButton* HelmButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HelmText;

	UPROPERTY(meta = (BindWidget))
	UButton* BodyButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BodyText;

	UPROPERTY(meta = (BindWidget))
	UButton* GauntletsButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GauntletsText;

	UPROPERTY(meta = (BindWidget))
	UButton* PantsButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PantsText;

	UPROPERTY(meta = (BindWidget))
	UButton* BootsButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BootsText;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* HotkeyTab;

	UPROPERTY(meta = (BindWidget))
	UButton* Hotkey_1Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Hotkey_1Text;

	UPROPERTY(meta = (BindWidget))
	UButton* Hotkey_2Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Hotkey_2Text;

	UPROPERTY(meta = (BindWidget))
	UButton* Hotkey_3Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Hotkey_3Text;

	UPROPERTY(meta = (BindWidget))
	UButton* Hotkey_4Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Hotkey_4Text;

	UPROPERTY(meta = (BindWidget))
	UButton* Hotkey_5Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Hotkey_5Text;

	UPROPERTY(meta = (BindWidget))
	class UInventoryDetailWidget* ConsumableDetailWidget;

	UPROPERTY(meta = (BindWidget))
	UEquipmentDetailWidget* FocusedEquipmentDetailWidget;

	UPROPERTY(meta = (BindWidget))
	UEquipmentDetailWidget* CurrentEquippedDetailWidget;

	EEquipmentSlotType CurrentSlotType;
	FInventoryItem CurrentEquippedItem;

	TMap<EEquipmentSlotType, UButton*> TabMap;
	TMap<EEquipmentSlotType, UTextBlock*> TextMap;
};
