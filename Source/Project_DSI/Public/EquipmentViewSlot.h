// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "InventoryManager.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentViewSlot.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentSlotHovered, FInventoryItem, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentSlotUnhovered);

UCLASS()
class PROJECT_DSI_API UEquipmentViewSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetEquipmentSlot(const FInventoryItem& InInventoryItem, const FItemData& InItemData);

	void ClearSlot();

	UFUNCTION()
	void OnSlotButtonClicked();

	UFUNCTION()
	void OnSlotButtonHovered();

	UFUNCTION()
	void OnSlotButtonUnhovered();

	bool IsInventoryItemValid() { return (InventoryItem.StaticStruct() != nullptr); }
	bool IsItemDataValid() { return (ItemData.StaticStruct() != nullptr); }

	FOnEquipmentSlotHovered OnEquipmentSlotHovered;
	FOnEquipmentSlotUnhovered OnEquipmentSlotUnhovered;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ItemCountOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	FItemData ItemData;

	UPROPERTY(BlueprintReadOnly)
	FInventoryItem InventoryItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEquipmentSlotType SlotType;
};
