// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentViewWidget.h"
#include "EquipmentViewSlot.h"
#include "EquipmentDetailWidget.h"
#include "InventoryDetailWidget.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "PlayerStatDetailWidget.h"
#include "PlayerHUD.h"

void UEquipmentViewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitSlotMap();

	EquipmentDetailWidget->SetVisibility(ESlateVisibility::Hidden);
	ConsumableDetailWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UEquipmentViewWidget::InitSlotMap()
{
	SlotMap.Add(EEquipmentSlotType::Weapon, WeaponSlot);
	SlotMap.Add(EEquipmentSlotType::Head, HeadSlot);
	SlotMap.Add(EEquipmentSlotType::Body, BodySlot);
	SlotMap.Add(EEquipmentSlotType::Glove, GloveSlot);
	SlotMap.Add(EEquipmentSlotType::Leg, LegSlot);
	SlotMap.Add(EEquipmentSlotType::Foot, FootSlot);

	SlotMap.Add(EEquipmentSlotType::Hotkey_1, HotkeySlot_1);
	SlotMap.Add(EEquipmentSlotType::Hotkey_2, HotkeySlot_2);
	SlotMap.Add(EEquipmentSlotType::Hotkey_3, HotkeySlot_3);
	SlotMap.Add(EEquipmentSlotType::Hotkey_4, HotkeySlot_4);
	SlotMap.Add(EEquipmentSlotType::Hotkey_5, HotkeySlot_5);
	//SlotMap.Add(EEquipmentSlotType::Hotkey_6, HotkeySlot_6);

	for (const auto& Pair : SlotMap)
	{
		const EEquipmentSlotType SlotType = Pair.Key;
		UEquipmentViewSlot* SlotWidget = Pair.Value;

		if (SlotType >= EEquipmentSlotType::Hotkey_1)
		{
			SlotWidget->OnEquipmentSlotHovered.AddDynamic(this, &UEquipmentViewWidget::SetConsumableDetailWidget);
			SlotWidget->OnEquipmentSlotUnhovered.AddDynamic(this, &UEquipmentViewWidget::HideConsumableDetailWidget);
		}
		else
		{
			SlotWidget->OnEquipmentSlotHovered.AddDynamic(this, &UEquipmentViewWidget::SetEquipmentDetailWidget);
			SlotWidget->OnEquipmentSlotUnhovered.AddDynamic(this, &UEquipmentViewWidget::HideEquipmentDetailWidget);
		}
	}
}

void UEquipmentViewWidget::LoadEquipment(UInventoryManager* Inventory)
{
	// 인벤토리가 존재하는지 체크
	if (!Inventory) return;

	// 장비창 슬롯 타입과 아이템의 TMap 변수 (TMap<EEquipmentSlotType, FInventoryItem>)
	const auto& EquippedMap = Inventory->GetEquippedItems();

	// 장비창 슬롯 타입과 장비 슬롯의 TMap 변수 (TMap<EEquipmentSlotType, UEquipmentViewSlot>)
	for (const auto& Pair : SlotMap)
	{
		const EEquipmentSlotType SlotType = Pair.Key;
		UEquipmentViewSlot* SlotWidget = Pair.Value;

		// 해당하는 슬롯 타입에 아이템이 있는지 체크;
		const FInventoryItem* Item = EquippedMap.Find(SlotType);
		if (!Item || Item->ItemID.IsNone())
		{
			// 장비 슬롯 초기화 (데이터 초기화)
			SlotWidget->ClearSlot();
			continue;
		}

		// 아이템 데이터 로드
		const FItemData* Data = Inventory->GetItemData(Item->ItemID);
		if (!Data)
		{
			// 데이터가 없으면 슬롯 초기화
			SlotWidget->ClearSlot();
			continue;
		}

		// 슬롯 세팅 (아이템 데이터 세팅)
		SlotWidget->SetEquipmentSlot(*Item, *Data);
	}

	// 플레이어 스탯 디테일 패널 세팅 (수치 업데이트)
	PlayerStatDetailWidget->SetStatDetail();

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	// HUD에 표시되는 핫키 아이템 로드
	Player->PlayerHUDWidget->RefreshHotkey();
}

void UEquipmentViewWidget::SetConsumableDetailWidget(FInventoryItem Item)
{
	if (Item.ItemID.IsNone()) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	const FItemData* Data = Inventory->GetItemData(Item.ItemID);
	if (!Data) return;

	ConsumableDetailWidget->ShowItemDetails(Item, *Data);
}

void UEquipmentViewWidget::HideConsumableDetailWidget()
{
	ConsumableDetailWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UEquipmentViewWidget::SetEquipmentDetailWidget(FInventoryItem Item)
{
	if (Item.ItemID.IsNone()) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	const FItemData* Data = Inventory->GetItemData(Item.ItemID);
	if (!Data) return;

	EquipmentDetailWidget->ShowItemDetails(Item, *Data);

	const FWeaponData* WeaponData = Inventory->GetWeaponData(Item.ItemID);
	if (WeaponData)
	{
		EquipmentDetailWidget->SetWeaponStatText(*WeaponData);
		return;
	}

	const FArmorData* ArmorData = Inventory->GetArmorData(Item.ItemID);
	if (ArmorData)
	{
		EquipmentDetailWidget->SetArmorStatText(*ArmorData);
		return;
	}
}

void UEquipmentViewWidget::HideEquipmentDetailWidget()
{
	EquipmentDetailWidget->SetVisibility(ESlateVisibility::Hidden);
}
