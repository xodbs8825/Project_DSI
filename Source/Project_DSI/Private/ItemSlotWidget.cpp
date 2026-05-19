// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"
#include "InventoryItemPromptWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotButton->OnHovered.AddDynamic(this, &UItemSlotWidget::ButtonHovered);
	SlotButton->OnUnhovered.AddDynamic(this, &UItemSlotWidget::ButtonUnhovered);
	SlotButton->OnReleased.AddDynamic(this, &UItemSlotWidget::ButtonClicked);
}

void UItemSlotWidget::SetItemSlot(const FInventoryItem& InInventoryItem, const FItemData& InItemData)
{
	InventoryItem = InInventoryItem;
	if (!IsInventoryItemValid()) return;

	ItemData = InItemData;
	if (!IsItemDataValid()) return;

	SlotButton->SetVisibility(ESlateVisibility::Visible);

	//ItemName->SetText(ItemData.ItemName);
	ItemCount->SetText(FText::AsNumber(InventoryItem.Count));

	UTexture2D* Image = ItemData.ItemIcon;
	if (Image)
	{
		ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
		//ItemName->SetVisibility(ESlateVisibility::Hidden);
	}

	SetVisibility(ESlateVisibility::Visible);
	//Frame->SetVisibility(ESlateVisibility::Hidden);
}

void UItemSlotWidget::SetIsSelected(bool bValue)
{
	//Frame->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UItemSlotWidget::ClearSlot()
{
	SlotButton->SetVisibility(ESlateVisibility::Hidden);
}

void UItemSlotWidget::ButtonHovered()
{
	if (InventoryItem.StaticStruct() && ItemData.StaticStruct())
	{
		OnItemSlotHovered.Broadcast(this);
	}
}

void UItemSlotWidget::ButtonUnhovered()
{
	OnItemSlotUnhovered.Broadcast();
}

void UItemSlotWidget::ButtonClicked()
{
	if (InventoryItem.StaticStruct() && ItemData.StaticStruct())
	{
		OnItemSlotClicked.Broadcast(this);
	}
}
