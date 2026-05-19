// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentViewSlot.h"
#include "ConsumableBaseItem.h"
#include "InventoryManager.h"
#include "BaseCharacter.h"
#include "ABPlayerController.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UEquipmentViewSlot::NativeConstruct()
{
	Super::NativeConstruct();

	SlotButton->OnClicked.AddDynamic(this, &UEquipmentViewSlot::OnSlotButtonClicked);
	SlotButton->OnHovered.AddDynamic(this, &UEquipmentViewSlot::OnSlotButtonHovered);
	SlotButton->OnUnhovered.AddDynamic(this, &UEquipmentViewSlot::OnSlotButtonUnhovered);
}

void UEquipmentViewSlot::SetEquipmentSlot(const FInventoryItem& InInventoryItem, const FItemData& InItemData)
{
	InventoryItem = InInventoryItem;
	if (!IsInventoryItemValid()) return;

	ItemData = InItemData;
	if (!IsItemDataValid()) return;

	UTexture2D* Image = ItemData.ItemIcon;
	if (Image)
	{
		ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
	}

	SetVisibility(ESlateVisibility::Visible);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);

	if (ItemData.ItemType == EItemType::Consumable)
	{
		if (InventoryItem.Count >= 2)
		{
			ItemCountText->SetText(FText::AsNumber(InventoryItem.Count));
			ItemCountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemCountOverlay->SetVisibility(ESlateVisibility::Visible);
			ItemCountText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		ItemCountOverlay->SetVisibility(ESlateVisibility::Hidden);
		ItemCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEquipmentViewSlot::ClearSlot()
{
	InventoryItem = FInventoryItem();
	ItemData = FItemData();

	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	ItemCountOverlay->SetVisibility(ESlateVisibility::Hidden);

	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::GetEmpty());
	}
}

void UEquipmentViewSlot::OnSlotButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	ABPC->OpenEquipItemUI(Inventory, SlotType);
}

void UEquipmentViewSlot::OnSlotButtonHovered()
{
	OnEquipmentSlotHovered.Broadcast(InventoryItem);
}

void UEquipmentViewSlot::OnSlotButtonUnhovered()
{
	OnEquipmentSlotUnhovered.Broadcast();
}
