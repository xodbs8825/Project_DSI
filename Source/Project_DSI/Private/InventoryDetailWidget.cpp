// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryDetailWidget.h"
#include "BaseItem.h"
#include "InventoryManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInventoryDetailWidget::ShowItemDetails(const FInventoryItem& Item, const FItemData& ItemData)
{
	ItemNameText->SetText(ItemData.ItemName);
	ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
	ItemCountText->SetText(FText::AsNumber(Item.Count));
	ItemMaxCountText->SetText(FText::AsNumber(ItemData.ItemMaxStack));
	ItemDescriptionText->SetText(ItemData.ItemDescription);

	SetVisibility(ESlateVisibility::Visible);
}
