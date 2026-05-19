// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipItemWidget.h"
#include "InventoryManager.h"
#include "EquipItemSlot.h"
#include "BaseCharacter.h"
#include "ABPlayerController.h"
#include "InventoryDetailWidget.h"
#include "EquipmentDetailWidget.h"
#include "PlayerStatDetailWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEquipItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitTabs();

	ConsumableDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	FocusedEquipmentDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	CurrentEquippedDetailWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UEquipItemWidget::InitTabs()
{
	TabMap.Add(EEquipmentSlotType::Head, HelmButton);
	TabMap.Add(EEquipmentSlotType::Body, BodyButton);
	TabMap.Add(EEquipmentSlotType::Glove, GauntletsButton);
	TabMap.Add(EEquipmentSlotType::Leg, PantsButton);
	TabMap.Add(EEquipmentSlotType::Foot, BootsButton);

	TabMap.Add(EEquipmentSlotType::Hotkey_1, Hotkey_1Button);
	TabMap.Add(EEquipmentSlotType::Hotkey_2, Hotkey_2Button);
	TabMap.Add(EEquipmentSlotType::Hotkey_3, Hotkey_3Button);
	TabMap.Add(EEquipmentSlotType::Hotkey_4, Hotkey_4Button);
	TabMap.Add(EEquipmentSlotType::Hotkey_5, Hotkey_5Button);

	TextMap.Add(EEquipmentSlotType::Head, HelmText);
	TextMap.Add(EEquipmentSlotType::Body, BodyText);
	TextMap.Add(EEquipmentSlotType::Glove, GauntletsText);
	TextMap.Add(EEquipmentSlotType::Leg, PantsText);
	TextMap.Add(EEquipmentSlotType::Foot, BootsText);

	TextMap.Add(EEquipmentSlotType::Hotkey_1, Hotkey_1Text);
	TextMap.Add(EEquipmentSlotType::Hotkey_2, Hotkey_2Text);
	TextMap.Add(EEquipmentSlotType::Hotkey_3, Hotkey_3Text);
	TextMap.Add(EEquipmentSlotType::Hotkey_4, Hotkey_4Text);
	TextMap.Add(EEquipmentSlotType::Hotkey_5, Hotkey_5Text);

	for (const auto& Pair : TabMap)
	{
		const EEquipmentSlotType Type = Pair.Key;
		UButton* Button = Pair.Value;

		switch (Type)
		{
		case EEquipmentSlotType::Head:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHelmsClicked);
			break;

		case EEquipmentSlotType::Body:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnBodyClicked);
			break;

		case EEquipmentSlotType::Glove:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnGauntletsClicked);
			break;

		case EEquipmentSlotType::Leg:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnPantsClicked);
			break;

		case EEquipmentSlotType::Foot:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnBootsClicked);
			break;

		case EEquipmentSlotType::Hotkey_1:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_1Clicked);
			break;

		case EEquipmentSlotType::Hotkey_2:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_2Clicked);
			break;

		case EEquipmentSlotType::Hotkey_3:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_3Clicked);
			break;

		case EEquipmentSlotType::Hotkey_4:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_4Clicked);
			break;

		case EEquipmentSlotType::Hotkey_5:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_5Clicked);
			break;

		case EEquipmentSlotType::Hotkey_6:
			Button->OnClicked.AddDynamic(this, &UEquipItemWidget::OnHotkey_6Clicked);
			break;

		}
	}
}

void UEquipItemWidget::RefreshList(UInventoryManager* Inventory, EEquipmentSlotType TargetSlotType)
{
	// 아이템 탭 Visibility 세팅
	if (TargetSlotType >= EEquipmentSlotType::Hotkey_1)
	{
		ArmorTab->SetVisibility(ESlateVisibility::Hidden);
		HotkeyTab->SetVisibility(ESlateVisibility::Visible);
	}
	else if (TargetSlotType == EEquipmentSlotType::Weapon)
	{
		ArmorTab->SetVisibility(ESlateVisibility::Hidden);
		HotkeyTab->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ArmorTab->SetVisibility(ESlateVisibility::Visible);
		HotkeyTab->SetVisibility(ESlateVisibility::Hidden);
	}

	// 디테일 패널 안보이게 설정
	ConsumableDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	FocusedEquipmentDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	CurrentEquippedDetailWidget->SetVisibility(ESlateVisibility::Hidden);

	// 아이템 리스트 비우기
	ItemListPanel->ClearChildren();

	// 현재 인벤토리에 존재하는 아이템 배열 불러오기
	TArray<FInventoryItem> Items = Inventory->GetItemsByType(GetItemType(TargetSlotType));
	//if (Items.IsEmpty()) return;

	int32 ItemCount = Items.Num();
	int32 RowCount = FMath::Max(CurrentRowCount, FMath::CeilToInt((float)ItemCount / ColumnCount));
	int32 TotalSlotCount = RowCount * ColumnCount;

	for (int32 i = 0; i < TotalSlotCount; i++)
	{
		UEquipItemSlot* ItemSlot = CreateWidget<UEquipItemSlot>(this, EquipSlotWidgetClass);
		if (!ItemSlot) return;

		if (i < ItemCount)
		{
			// 아이템 배열에 존재하는 아이템들의 데이터 불러오기
			const FItemData* Data = Inventory->GetItemData(Items[i].ItemID);
			//if (!Data) continue;

			//if (!IsValidForSlot(TargetSlotType, Data->ItemType)) continue;

			// 아이템 슬롯 Create
			ItemSlot->InitSlot(this, Items[i], *Data);

			bool IsEquipped = Inventory->IsItemEquipped(Items[i]);
			ItemSlot->SetEquipped(IsEquipped);

			if (IsEquipped) CurrentEquippedItem = Items[i];
		}
		else
		{
			ItemSlot->ClearSlot();
		}

		// 슬롯 추가
		ItemListPanel->AddChild(ItemSlot);
	}

	//for (const FInventoryItem& Item : Items)
	//{
	//	// 아이템 배열에 존재하는 아이템들의 데이터 불러오기
	//	const FItemData* Data = Inventory->GetItemData(Item.ItemID);
	//	if (!Data) continue;

	//	if (!IsValidForSlot(TargetSlotType, Data->ItemType)) continue;

	//	// 아이템 슬롯 Create
	//	CreateItemSlot(Item, *Data);
	//}
}

bool UEquipItemWidget::IsValidForSlot(EEquipmentSlotType SlotType, EItemType ItemType)
{
	switch (SlotType)
	{
	case EEquipmentSlotType::Weapon:
		return ItemType == EItemType::Weapon;

	case EEquipmentSlotType::Head:
		return ItemType == EItemType::HeadArmor;

	case EEquipmentSlotType::Body:
		return ItemType == EItemType::BodyArmor;

	case EEquipmentSlotType::Glove:
		return ItemType == EItemType::GloveArmor;

	case EEquipmentSlotType::Leg:
		return ItemType == EItemType::LegArmor;

	case EEquipmentSlotType::Foot:
		return ItemType == EItemType::FootArmor;

	case EEquipmentSlotType::Hotkey_1:
	case EEquipmentSlotType::Hotkey_2:
	case EEquipmentSlotType::Hotkey_3:
	case EEquipmentSlotType::Hotkey_4:
	case EEquipmentSlotType::Hotkey_5:
	case EEquipmentSlotType::Hotkey_6:
		return ItemType == EItemType::Consumable;
	}

	return false;
}

EItemType UEquipItemWidget::GetItemType(EEquipmentSlotType SlotType)
{
	EItemType Type;

	switch (SlotType)
	{
	case EEquipmentSlotType::Weapon:
		Type = EItemType::Weapon;
		break;

	case EEquipmentSlotType::Head:
		Type = EItemType::HeadArmor;
		break;

	case EEquipmentSlotType::Body:
		Type = EItemType::BodyArmor;
		break;

	case EEquipmentSlotType::Glove:
		Type = EItemType::GloveArmor;
		break;

	case EEquipmentSlotType::Leg:
		Type = EItemType::LegArmor;
		break;

	case EEquipmentSlotType::Foot:
		Type = EItemType::FootArmor;
		break;

	case EEquipmentSlotType::Hotkey_1:
	case EEquipmentSlotType::Hotkey_2:
	case EEquipmentSlotType::Hotkey_3:
	case EEquipmentSlotType::Hotkey_4:
	case EEquipmentSlotType::Hotkey_5:
	case EEquipmentSlotType::Hotkey_6:
		Type = EItemType::Consumable;
		break;
	}

	return Type;
}

void UEquipItemWidget::CreateItemSlot(const FInventoryItem& Item, const FItemData& Data)
{
	if (!EquipSlotWidgetClass || !ItemListPanel) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	UEquipItemSlot* ItemSlot = CreateWidget<UEquipItemSlot>(this, EquipSlotWidgetClass);
	if (!ItemSlot) return;

	ItemSlot->InitSlot(this, Item, Data);

	bool IsEquipped = Inventory->IsItemEquipped(Item);
	ItemSlot->SetEquipped(IsEquipped);

	if (IsEquipped) CurrentEquippedItem = Item;

	ItemListPanel->AddChild(ItemSlot);
}

void UEquipItemWidget::OnItemSelected(const FInventoryItem& Item)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	Inventory->EquipItem(CurrentSlotType, Item);

	ABPC->CloseEquipItemUI(Inventory);
}

void UEquipItemWidget::OnTabSelected(EEquipmentSlotType Type)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	CurrentSlotType = Type;

	RefreshList(Inventory, Type);

	for (const auto& Pair : TabMap)
	{
		const EEquipmentSlotType SlotType = Pair.Key;
		UButton* Button = Pair.Value;
		UTextBlock* Text = *TextMap.Find(SlotType);

		SetIsTabFocused((Type == SlotType), Button, Text);
	}
}

void UEquipItemWidget::SetIsTabFocused(bool bValue, UButton* TabButton, UTextBlock* TabText)
{
	if (bValue)
	{
		FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#DADA00FF")));
		TabButton->SetBackgroundColor(Yellow);
		TabText->SetColorAndOpacity(Yellow);
	}
	else
	{
		FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));
		TabButton->SetBackgroundColor(White);
		TabText->SetColorAndOpacity(White);
	}
}

void UEquipItemWidget::OnHelmsClicked()
{
	OnTabSelected(EEquipmentSlotType::Head);
}

void UEquipItemWidget::OnBodyClicked()
{
	OnTabSelected(EEquipmentSlotType::Body);
}

void UEquipItemWidget::OnGauntletsClicked()
{
	OnTabSelected(EEquipmentSlotType::Glove);
}

void UEquipItemWidget::OnPantsClicked()
{
	OnTabSelected(EEquipmentSlotType::Leg);
}

void UEquipItemWidget::OnBootsClicked()
{
	OnTabSelected(EEquipmentSlotType::Foot);
}

void UEquipItemWidget::OnHotkey_1Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_1);
}

void UEquipItemWidget::OnHotkey_2Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_2);
}

void UEquipItemWidget::OnHotkey_3Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_3);
}

void UEquipItemWidget::OnHotkey_4Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_4);
}

void UEquipItemWidget::OnHotkey_5Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_5);
}

void UEquipItemWidget::OnHotkey_6Clicked()
{
	OnTabSelected(EEquipmentSlotType::Hotkey_6);
}
