// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemDetailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "BaseItem.h"
#include "ConsumableBaseItem.h"
#include "MerchantNPC.h"
#include "BaseCharacter.h"
#include "InventoryManager.h"
#include "Components/Image.h"

void UShopItemDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(QuantityUpBtn);
	check(QuantityDownBtn);

	SetPurchaseCountText(1);

	QuantityUpBtn->OnClicked.AddDynamic(this, &UShopItemDetailWidget::OnQuantityUpButtonClicked);
	QuantityDownBtn->OnClicked.AddDynamic(this, &UShopItemDetailWidget::OnQuantityDownButtonClicked);
}

void UShopItemDetailWidget::OnQuantityUpButtonClicked()
{
	OnPurchaseQuantityUp.Broadcast();
}

void UShopItemDetailWidget::OnQuantityDownButtonClicked()
{
	OnPurchaseQuantityDown.Broadcast();
}

void UShopItemDetailWidget::SetPurchaseCountText(int32 Count)
{
	PurchaseCount->SetText(FText::AsNumber(Count));
}

void UShopItemDetailWidget::ShowPurchaseCount()
{
	PurchaseCountContainer->SetVisibility(ESlateVisibility::Visible);
}

void UShopItemDetailWidget::HidePurchaseCount()
{
	PurchaseCountContainer->SetVisibility(ESlateVisibility::Hidden);
}

void UShopItemDetailWidget::SetItemQuantityVisibility(ESlateVisibility NewVisibility)
{
	ItemQuantityContainer->SetVisibility(NewVisibility);
}

void UShopItemDetailWidget::EnablePurchaseCounter(bool bValue)
{
	QuantityUpBtn->SetIsEnabled(bValue);
	QuantityDownBtn->SetIsEnabled(bValue);
}

void UShopItemDetailWidget::ShowItem(const FShopItem& Item, const FItemData& ItemData)
{
	SetVisibility(ESlateVisibility::Visible);

	FText TypeText = StaticEnum<EItemType>()->GetDisplayNameTextByValue(static_cast<int64>(ItemData.ItemType));

	ItemName->SetText(ItemData.ItemName);
	ItemDescription->SetText(ItemData.ItemDescription);
	ItemTypeText->SetText(TypeText);
	ItemPrice->SetText(FText::AsNumber(Item.Price));

	ItemImage->SetBrushFromTexture(ItemData.ItemIcon);

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	FInventoryItem InvenItem = Inventory->GetItemByItemID(Item.ItemID);
	ItemCount->SetText(FText::AsNumber(InvenItem.Count));
	ItemMaxCount->SetText(FText::AsNumber(ItemData.ItemMaxStack));
}

void UShopItemDetailWidget::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
