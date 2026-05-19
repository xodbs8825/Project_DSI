// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEntryWidget.h"
#include "InventoryManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemEntryWidget::SetItemEntry(const FItemData& ItemData, int32 Count)
{
	ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
	ItemName->SetText(ItemData.ItemName);
	ItemCount->SetText(FText::AsNumber(Count));
}
