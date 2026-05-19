// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDHotkeySlotWidget.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UHUDHotkeySlotWidget::SetSlot(UPlayerHUD* HUD, FInventoryItem Item)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	const FItemData* Data = Inventory->GetItemData(Item.ItemID);
	if (!Data) return;

	ItemImage->SetBrushFromTexture(Data->ItemIcon);
	ItemImage->SetVisibility(ESlateVisibility::Visible);

	ItemCountOverlay->SetVisibility(ESlateVisibility::Visible);
	ItemCount->SetText(FText::AsNumber(Item.Count));
}

void UHUDHotkeySlotWidget::ClearSlot()
{
	ItemImage->SetBrushFromTexture(nullptr);
	ItemImage->SetVisibility(ESlateVisibility::Hidden);

	ItemCountOverlay->SetVisibility(ESlateVisibility::Hidden);
	ItemCount->SetText(FText::GetEmpty());
}