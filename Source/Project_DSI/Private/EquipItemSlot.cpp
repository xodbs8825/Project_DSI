// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipItemSlot.h"
#include "EquipItemWidget.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "InventoryDetailWidget.h"
#include "EquipmentDetailWidget.h"
#include "InventoryManager.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEquipItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void UEquipItemSlot::InitSlot(UEquipItemWidget* InOwner, const FInventoryItem& InItem, const FItemData& InData)
{
	OwnerWidget = InOwner;
	Item = InItem;
	ItemData = InData;

	if (InItem.Count > 1)
	{
		ItemCountOverlay->SetVisibility(ESlateVisibility::Visible);
		ItemCount->SetText(FText::AsNumber(InItem.Count));
	}
	else
	{
		ItemCountOverlay->SetVisibility(ESlateVisibility::Hidden);
	}

	if (InData.ItemIcon)
	{
		ItemImage->SetBrushFromTexture(InData.ItemIcon);
	}

	ItemButton->OnClicked.AddDynamic(this, &UEquipItemSlot::OnButtonClicked);
	ItemButton->OnHovered.AddDynamic(this, &UEquipItemSlot::OnButtonHovered);
	ItemButton->OnUnhovered.AddDynamic(this, &UEquipItemSlot::OnButtonUnhovered);

	EquipIndicator->SetVisibility(ESlateVisibility::Hidden);

	if (InData.ItemType != EItemType::Consumable)
	{
		ItemCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEquipItemSlot::SetEquipped(bool IsEquipped)
{
	EquipIndicator->SetVisibility(IsEquipped ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UEquipItemSlot::ClearSlot()
{
	ItemButton->SetVisibility(ESlateVisibility::Hidden);
}

void UEquipItemSlot::OnButtonClicked()
{
	if (!OwnerWidget) return;
	OwnerWidget->OnItemSelected(Item);
}

void UEquipItemSlot::OnButtonHovered()
{
	if (!OwnerWidget) return;

	EEquipmentSlotType SlotType = OwnerWidget->CurrentSlotType;

	if (SlotType >= EEquipmentSlotType::Hotkey_1)
	{
		OwnerWidget->ConsumableDetailWidget->ShowItemDetails(Item, ItemData);
	}
	else
	{
		OwnerWidget->FocusedEquipmentDetailWidget->ShowItemDetails(Item, ItemData);

		ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
		if (!Player) return;

		UInventoryManager* Inventory = Player->InventoryManager;
		if (!Inventory) return;

		if (SlotType == EEquipmentSlotType::Weapon)
		{
			const FWeaponData* WeaponData = Inventory->GetWeaponData(Item.ItemID);
			if (WeaponData)
			{
				OwnerWidget->FocusedEquipmentDetailWidget->SetWeaponStatText(*WeaponData);
			}
		}
		else
		{
			const FArmorData* ArmorData = Inventory->GetArmorData(Item.ItemID);
			if (ArmorData)
			{
				OwnerWidget->FocusedEquipmentDetailWidget->SetArmorStatText(*ArmorData);
			}
		}

		FInventoryItem EquippedItem = OwnerWidget->CurrentEquippedItem;
		if (EquippedItem.ItemID.IsNone()) return;

		if (Item.ItemID != EquippedItem.ItemID)
		{
			const FItemData* Data = Inventory->GetItemData(EquippedItem.ItemID);
			if (!Data) return;

			OwnerWidget->CurrentEquippedDetailWidget->ShowItemDetails(EquippedItem, *Data);

			if (SlotType == EEquipmentSlotType::Weapon)
			{
				const FWeaponData* WeaponData = Inventory->GetWeaponData(EquippedItem.ItemID);
				if (WeaponData)
				{
					OwnerWidget->CurrentEquippedDetailWidget->SetWeaponStatText(*WeaponData);
				}
			}
			else
			{
				const FArmorData* ArmorData = Inventory->GetArmorData(EquippedItem.ItemID);
				if (ArmorData)
				{
					OwnerWidget->CurrentEquippedDetailWidget->SetArmorStatText(*ArmorData);
				}
			}
		}
	}
}

void UEquipItemSlot::OnButtonUnhovered()
{
	OwnerWidget->ConsumableDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	OwnerWidget->FocusedEquipmentDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	OwnerWidget->CurrentEquippedDetailWidget->SetVisibility(ESlateVisibility::Hidden);
}