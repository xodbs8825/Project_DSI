// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "InventoryManager.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

struct FInventoryItem;
struct FItemData;

class UButton;
class UImage;
class UTextBlock;

class UInventoryItemPromptWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotHovered, UItemSlotWidget*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSlotUnhovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, UItemSlotWidget*, Slot);

UCLASS()
class PROJECT_DSI_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetItemSlot(const FInventoryItem& InInventoryItem, const FItemData& InItemData);
	void SetIsSelected(bool bValue);

	void ClearSlot();

	UFUNCTION()
	void ButtonHovered();

	UFUNCTION()
	void ButtonUnhovered();

	UFUNCTION()
	void ButtonClicked();

	FInventoryItem* GetInventoryItem() { return &InventoryItem; }
	FItemData* GetItemData() { return &ItemData; }

	bool IsItemDataValid() { return (ItemData.StaticStruct() != nullptr); }
	bool IsInventoryItemValid() { return (InventoryItem.StaticStruct() != nullptr); }

public:
	FOnItemSlotHovered OnItemSlotHovered;
	FOnItemSlotUnhovered OnItemSlotUnhovered;
	FOnItemSlotClicked OnItemSlotClicked;

public:
	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	// Temp
	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCount;

	//UPROPERTY(meta = (BindWidget))
	//UImage* Frame;

	UPROPERTY(BlueprintReadOnly, Category = "Item Data")
	FItemData ItemData;

	UPROPERTY(BlueprintReadOnly, Category = "Item Data")
	FInventoryItem InventoryItem;
};
