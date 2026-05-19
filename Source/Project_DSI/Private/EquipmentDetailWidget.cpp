// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentDetailWidget.h"
#include "BaseItem.h"
#include "WeaponBaseItem.h"
#include "InventoryManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UEquipmentDetailWidget::ShowItemDetails(const FInventoryItem& Item, const FItemData& ItemData)
{
	ItemNameText->SetText(ItemData.ItemName);
	ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
	ItemDescriptionText->SetText(ItemData.ItemDescription);

	SetVisibility(ESlateVisibility::Visible);
}

void UEquipmentDetailWidget::SetWeaponStatText(const FWeaponData& Weapon)
{
	ItemStatText->SetText(FText::AsNumber(Weapon.BaseDamage));
	StatTypeText->SetText(FText::FromString(TEXT("Damage")));
}

void UEquipmentDetailWidget::SetArmorStatText(const FArmorData& Armor)
{
	ItemStatText->SetText(FText::AsNumber(Armor.BaseDefence));
	StatTypeText->SetText(FText::FromString(TEXT("Defence")));
}
