// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "EquipItemSlot.generated.h"

class UEquipItemWidget;

class UImage;
class UButton;
class UTextBlock;

UCLASS()
class PROJECT_DSI_API UEquipItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitSlot(UEquipItemWidget* InOwner, const FInventoryItem& InItem, const FItemData& InData);
	
	void SetEquipped(bool IsEquipped);

	void ClearSlot();

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonUnhovered();

public:
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ItemCountOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCount;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* EquipIndicator;

	UEquipItemWidget* OwnerWidget;

	FInventoryItem Item;
	FItemData ItemData;
};
