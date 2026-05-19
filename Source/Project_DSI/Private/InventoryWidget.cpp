// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "ItemSlotWidget.h"
#include "ItemTypeTabWidget.h"
#include "BaseCharacter.h"
#include "InventoryDetailWidget.h"
#include "InventoryItemPromptWidget.h"
#include "Components/PanelWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Overlay.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PromptWidgetClass)
	{
		PromptWidget = CreateWidget<UInventoryItemPromptWidget>(this, PromptWidgetClass);

		if (PromptWidget)
		{
			PromptWidget->AddToViewport(1000);
			PromptWidget->SetVisibility(ESlateVisibility::Hidden);
			PromptWidget->InventoryWidget = this;
		}
	}

	ConsumableTab->OnItemTypeChanged.AddDynamic(this, &UInventoryWidget::ItemTypeChanged);
	KeyItemTab->OnItemTypeChanged.AddDynamic(this, &UInventoryWidget::ItemTypeChanged);
	UpgradeMaterialTab->OnItemTypeChanged.AddDynamic(this, &UInventoryWidget::ItemTypeChanged);

	ItemTypeMap.Add(EItemType::Consumable, ConsumableTab);
	ItemTypeMap.Add(EItemType::KeyItem, KeyItemTab);
	ItemTypeMap.Add(EItemType::UpgradeMaterial, UpgradeMaterialTab);

	InitInventoryWidget();
}

void UInventoryWidget::InitInventoryWidget()
{
	FocusedItem = nullptr;
	SelectedSlot = nullptr;
	FocusState = EItemSlotFocusState::None;

	DetailWidget->SetVisibility(ESlateVisibility::Hidden);
	PromptWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::SetInventoryItems(const TArray<FInventoryItem> InventoryItems)
{
	CurrentItemType = EItemType::Consumable;

	ConsumableTab->SetIsFocused(true);
	KeyItemTab->SetIsFocused(false);
	UpgradeMaterialTab->SetIsFocused(false);
}

void UInventoryWidget::LoadInventory(UInventoryManager* Inventory, EItemType ItemType)
{
	// 아이템 슬롯 위젯 클래스가 있는지 확인
	if (!ItemSlotWidgetClass) return;

	// 리스트 초기화
	ItemListPanel->ClearChildren();

	// 아이템 가져오기
	TArray<FInventoryItem> Items = Inventory->GetItemsByType(ItemType);
	//if (Items.IsEmpty()) return;

	int32 ItemCount = Items.Num();
	int32 RowCount = FMath::Max(CurrentRowCount, FMath::CeilToInt((float)ItemCount / ColumnCount));
	int32 TotalSlotCount = RowCount * ColumnCount;

	for (int32 i = 0; i < TotalSlotCount; i++)
	{
		UItemSlotWidget* ItemSlot = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
		if (!ItemSlot) continue;

		if (i < ItemCount)
		{
			// 데이터 테이블에서 ItemID로 아이템 데이터 가져오기
			const FItemData* ItemData = Inventory->GetItemData(Items[i].ItemID);

			// 슬롯 세팅(아이템 정보 전달)
			ItemSlot->SetItemSlot(Items[i], *ItemData);

			// 슬롯 이벤트 바인딩
			ItemSlot->OnItemSlotHovered.AddDynamic(this, &UInventoryWidget::HandleHovered);
			ItemSlot->OnItemSlotUnhovered.AddDynamic(this, &UInventoryWidget::HandleUnhovered);
			ItemSlot->OnItemSlotClicked.AddDynamic(this, &UInventoryWidget::HandleClicked);
		}
		else
		{
			ItemSlot->ClearSlot();
		}

		// 슬롯 추가
		ItemListPanel->AddChild(ItemSlot);
	}

	//for (auto& Item : Items)
	//{
	//	// 슬롯 생성
	//	UItemSlotWidget* ItemSlot = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
	//	if (!ItemSlot) continue;

	//	// 데이터 테이블에서 ItemID로 아이템 데이터 가져오기
	//	const FItemData* ItemData = Inventory->GetItemData(Item.ItemID);

	//	// 슬롯 세팅(아이템 정보 전달)
	//	ItemSlot->SetItemSlot(Item, *ItemData);

	//	// 슬롯 이벤트 바인딩
	//	ItemSlot->OnItemSlotHovered.AddDynamic(this, &UInventoryWidget::HandleHovered);
	//	ItemSlot->OnItemSlotUnhovered.AddDynamic(this, &UInventoryWidget::HandleUnhovered);
	//	ItemSlot->OnItemSlotClicked.AddDynamic(this, &UInventoryWidget::HandleClicked);

	//	// 슬롯 추가
	//	ItemListPanel->AddChild(ItemSlot);
	//}
}

void UInventoryWidget::ShowPrompt(UItemSlotWidget* ClickedSlot)
{
	if (!PromptWidget || !ClickedSlot) return;

	FVector2D SlotPos;
	FVector2D SlotVPos;
	FVector2D SlotSize = ClickedSlot->GetCachedGeometry().GetLocalSize();

	USlateBlueprintLibrary::LocalToViewport(GetWorld(), ClickedSlot->GetCachedGeometry(), FVector2D(0, 0), SlotPos, SlotVPos);

	FVector2D NewPos = SlotVPos + SlotSize * 0.5f;
	PromptWidget->SetPositionInViewport(NewPos, false);

	PromptWidget->SetPromptWidget(ClickedSlot);
}

void UInventoryWidget::HandleHovered(UItemSlotWidget* HoveredSlot)
{
	if (FocusState == EItemSlotFocusState::None)
	{
		FocusedItem = HoveredSlot->GetInventoryItem();
		FocusState = EItemSlotFocusState::Hovered;

		FItemData* ItemData = HoveredSlot->GetItemData();

		DetailWidget->ShowItemDetails(*FocusedItem, *ItemData);
	}
}

void UInventoryWidget::HandleUnhovered()
{
	if (FocusState == EItemSlotFocusState::Hovered)
	{
		FocusedItem = nullptr;
		FocusState = EItemSlotFocusState::None;

		DetailWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::HandleClicked(UItemSlotWidget* ClickedSlot)
{
	if (SelectedSlot != ClickedSlot)
	{
		if (SelectedSlot != nullptr) SelectedSlot->SetIsSelected(false);

		SelectedSlot = ClickedSlot;
		SelectedSlot->SetIsSelected(true);

		FocusedItem = ClickedSlot->GetInventoryItem();
		FocusState = EItemSlotFocusState::Selected;

		FItemData* ItemData = ClickedSlot->GetItemData();
		if (ItemData->ItemType == EItemType::KeyItem) return;

		ShowPrompt(ClickedSlot);
	}
	else
	{
		if (!PromptWidget) return;

		SelectedSlot->SetIsSelected(false);
		SelectedSlot = nullptr;

		FocusState = EItemSlotFocusState::Hovered;
		PromptWidget->ResetPromptWidget(false);
	}
}

void UInventoryWidget::ItemTypeChanged(EItemType NewType)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	LoadInventory(Inventory, NewType);

	for (auto& Elem : ItemTypeMap)
	{
		if (NewType != Elem.Key)
		{
			Elem.Value->SetIsFocused(false);
		}
	}

	InitInventoryWidget();

	if (!DetailWidget) return;
	DetailWidget->SetVisibility(ESlateVisibility::Hidden);
}