// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemSlotWidget.h"
#include "BaseItem.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UShopItemSlotWidget::Init(const FShopItem& InShopItem, const FItemData& InItemData)
{
	ShopItem = InShopItem;
	if (ShopItem.StaticStruct() == nullptr) return;

	ItemData = InItemData;
	if (ItemData.StaticStruct() == nullptr) return;

	UTexture2D* Image = ItemData.ItemIcon;
	if (Image)
	{
		ItemImage->SetBrushFromTexture(Image);
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UShopItemSlotWidget::ClearSlot()
{
	SlotButton->SetVisibility(ESlateVisibility::Hidden);
}

void UShopItemSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bHolding || bHoldCompleted) return;

	HoldElapsed += InDeltaTime;

	float Ratio = HoldElapsed / RequiredHoldTime;

	OnItemHoldProgress.Broadcast(Ratio, this);

	if (Ratio >= 1.0f)
	{
		//bHoldCompleted = true;
		//bHolding = false;
		//HoldElapsed = 0.0f;

		OnItemHoldCompleted.Broadcast(this);
	}
}

void UShopItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(SlotButton);

	SlotButton->OnHovered.AddDynamic(this, &UShopItemSlotWidget::ButtonHovered);
	SlotButton->OnUnhovered.AddDynamic(this, &UShopItemSlotWidget::ButtonUnhovered);
	SlotButton->OnClicked.AddDynamic(this, &UShopItemSlotWidget::ButtonClicked);
	SlotButton->OnPressed.AddDynamic(this, &UShopItemSlotWidget::ButtonPressed);
	SlotButton->OnReleased.AddDynamic(this, &UShopItemSlotWidget::ButtonReleased);
}

void UShopItemSlotWidget::ButtonHovered()
{
	if (ShopItem.StaticStruct() && ItemData.StaticStruct())
	{
		OnItemHovered.Broadcast(this);
	}
}

void UShopItemSlotWidget::ButtonUnhovered()
{
	OnItemUnhovered.Broadcast();
}

void UShopItemSlotWidget::ButtonClicked()
{
	if (bHoldCompleted) return;

	if (ShopItem.StaticStruct() && ItemData.StaticStruct())
	{
		OnItemClicked.Broadcast(this);
	}
}

void UShopItemSlotWidget::ButtonPressed()
{
	bHolding = true;
	bHoldCompleted = false;

	HoldElapsed = 0.0f;
}

void UShopItemSlotWidget::ButtonReleased()
{
	if (bHoldCompleted) return;

	if (bHolding)
	{
		bHolding = false;
		HoldElapsed = 0.0f;

		OnItemHoldProgress.Broadcast(0.0f, this);
	}
}

void UShopItemSlotWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
}

void UShopItemSlotWidget::CancelHold()
{
	bHolding = false;
	bHoldCompleted = false;
	HoldElapsed = 0.0f;

	OnItemHoldProgress.Broadcast(0.0f, this);
}

void UShopItemSlotWidget::UpdateHoldProgressBar(float Ratio)
{
	HoldProgressBar->SetPercent(Ratio);
}
