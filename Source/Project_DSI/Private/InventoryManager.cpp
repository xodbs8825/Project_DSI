// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManager.h"
#include "ItemDropActor.h"
#include "BaseItemEffect.h"
#include "BaseCharacter.h"
#include "ConsumableBaseItem.h"
#include "WeaponBaseItem.h"
#include "ABCharacterStatComponent.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "PlayerHUD.h"

// Sets default values for this component's properties
UInventoryManager::UInventoryManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryManager::BeginPlay()
{
	Super::BeginPlay();

	LoadInventoryData();
	LoadEquipmentData();
}

// Called every frame
void UInventoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// 아이템 추가 (줍기)
void UInventoryManager::AddItem(FName ItemID, int32 Count)
{
	// 추가할 아이템 생성
	FInventoryItem NewItem;
	NewItem.ItemID = ItemID;
	NewItem.Count = Count;

	const FItemData* ItemData = GetItemData(ItemID);
	if (ItemData)
	{
		OnItemObtained.Broadcast(*ItemData, Count);
	}

	// 인벤토리에 추가할 아이템이 있는지 확인
	for (FInventoryItem& Item : Inventory)
	{
		// 인벤토리에 같은 아이템이 있으면 개수만 증가
		if (Item.ItemID == ItemID)
		{
			Item.Count += Count;
			NewItem.Count = Item.Count;

			// 장비 아이템 업데이트
			UpdateEquipment(NewItem);

			// 인벤토리 데이터 저장
			SaveInventoryData();

			return;
		}
	}

	// 인벤토리에 같은 아이템이 없으면 단순 추가
	Inventory.Add(NewItem);

	// 장비 아이템 업데이트
	UpdateEquipment(NewItem);

	// 인벤토리 데이터 저장
	SaveInventoryData();
}

// 아이템 사용은 소모품만 사용
bool UInventoryManager::UseItem(FName ItemID, int32 Count)
{
	// 데이터 테이블에서 아이템 데이터 불러오기
	const FItemData* ItemData = GetItemData(ItemID);
	if (!ItemData) return false;
	if (!ItemData->ItemClass) return false;

	// 소모품 아이템 캐스팅
	AConsumableBaseItem* Item = Cast<AConsumableBaseItem>(ItemData->ItemClass->GetDefaultObject());
	if (!Item) return false;

	// 소모품 데이터 불러오기
	const FConsumableData* ConsumableData = GetConsumableData(ItemID);
	if (!ConsumableData) return false;
	if (!ConsumableData->ItemEffect) return false;

	// 한번에 아이템을 두 개 이상 사용할 수 있는지 확인
	if (!ConsumableData->AllowMultiUse)
	{
		Count = 1;
	}

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwner());
	if (!Player) return false;

	// 아이템 효과 스폰하고 효과 적용
	ABaseItemEffect* Effect = GetWorld()->SpawnActor<ABaseItemEffect>(ConsumableData->ItemEffect, Player->GetActorLocation(), FRotator::ZeroRotator);
	if (Effect)
	{
		Effect->ApplyEffect(Player, Count);
	}

	// 아이템을 인벤토리에서 사용한 개수 만큼 제거
	RemoveItem(ItemID, Count);

	SaveInventoryData();

	Player->PlayerHUDWidget->RefreshHotkey();

	return true;
}

// 아이템 개수 감소 (아이템 폐기 기능으로도 사용)
bool UInventoryManager::RemoveItem(FName ItemID, int32 Count)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		// 인벤토리에 아이템이 있는지 확인
		if (Inventory[i].ItemID == ItemID)
		{
			// 개수 감소
			Inventory[i].Count -= Count;

			// 장비 아이템 업데이트
			UpdateEquipment(Inventory[i]);

			if (Inventory[i].Count <= 0)
			{
				// 아이템 인벤토리에서 완전 제거
				Inventory.RemoveAt(i);
			}

			return true;
		}
	}

	return false;
}

// 아이템 버리기
bool UInventoryManager::DropItem(FName ItemID, int32 Count)
{
	// 아이템이 인벤토리에 있으면 아이템 버리기
	if (!RemoveItem(ItemID, Count))
	{
		return false;
	}

	// 버릴 위치는 플레이어 캐릭터 발 밑
	FVector DropLocation = GetOwner()->GetActorLocation();
	DropLocation.Z = 10.0f;

	// 아이템 스폰
	AItemDropActor* DropActor = GetWorld()->SpawnActor<AItemDropActor>(DropActorClass, DropLocation, FRotator::ZeroRotator);
	if (DropActor)
	{
		DropActor->ItemID = ItemID;
		DropActor->Count = Count;
	}

	return true;
}

void UInventoryManager::EquipItem(EEquipmentSlotType SlotType, const FInventoryItem& Item)
{
	FInventoryItem* Current = EquippedItems.EquippedItemMap.Find(SlotType);

	if (Current && Current->ItemID == Item.ItemID)
	{
		EquippedItems.EquippedItemMap.Remove(SlotType);
		ClearItemStat(Item);
		SaveEquipmentData(FInventoryItem(), SlotType);

		return;
	}

	if (IsHotkeySlot(SlotType))
	{
		RemoveItemFromAllSlots(Item);
	}

	EquippedItems.EquippedItemMap.Add(SlotType, Item);
	UpdateItemStat(Item);
	SaveEquipmentData(Item, SlotType);
}

void UInventoryManager::RemoveItemFromAllSlots(const FInventoryItem& Item)
{
	TArray<EEquipmentSlotType> SlotsToRemove;

	for (const auto& Pair : EquippedItems.EquippedItemMap)
	{
		if (Pair.Value.ItemID == Item.ItemID)
		{
			SlotsToRemove.Add(Pair.Key);
		}
	}

	for (EEquipmentSlotType Slot : SlotsToRemove)
	{
		EquippedItems.EquippedItemMap.Remove(Slot);
		SaveEquipmentData(FInventoryItem(), Slot);
	}
}

bool UInventoryManager::IsItemEquipped(const FInventoryItem& Item) const
{
	for (const auto& Pair : EquippedItems.EquippedItemMap)
	{
		const FInventoryItem& Equipped = Pair.Value;

		if (Equipped.ItemID == Item.ItemID)
		{
			return true;
		}
	}

	return false;
}

bool UInventoryManager::IsHotkeySlot(EEquipmentSlotType SlotType)
{
	return SlotType >= EEquipmentSlotType::Hotkey_1;
}

void UInventoryManager::UpdateEquipment(const FInventoryItem& Item)
{
	for (const auto& Pair : EquippedItems.EquippedItemMap)
	{
		EEquipmentSlotType Type = Pair.Key;
		const FInventoryItem& Equipped = Pair.Value;

		if (Equipped.ItemID == Item.ItemID)
		{
			if (IsHotkeySlot(Type))
			{
				RemoveItemFromAllSlots(Item);
			}

			if (Item.Count <= 0) return;

			EquippedItems.EquippedItemMap.Add(Type, Item);
			SaveEquipmentData(Item, Type);
		}
	}
}

void UInventoryManager::UpdateItemStat(FInventoryItem Item)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwner());
	if (!Player) return;

	UABCharacterStatComponent* Stat = Player->CharacterStatManager;
	if (!Stat) return;

	const FWeaponData* WeaponData = GetWeaponData(Item.ItemID);
	if (WeaponData)
	{
		Stat->SetWeaponData(WeaponData);
		Player->SetWeapon(WeaponData);

		return;
	}

	const FArmorData* ArmorData = GetArmorData(Item.ItemID);
	if (ArmorData)
	{
		Stat->SetArmorData(ArmorData);
		return;
	}
}

void UInventoryManager::ClearItemStat(FInventoryItem Item)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwner());
	if (!Player) return;

	UABCharacterStatComponent* Stat = Player->CharacterStatManager;
	if (!Stat) return;

	const FWeaponData* WeaponData = GetWeaponData(Item.ItemID);
	if (WeaponData)
	{
		Stat->ClearWeaponData();
		Player->ClearWeapon();

		return;
	}

	const FArmorData* ArmorData = GetArmorData(Item.ItemID);
	if (ArmorData)
	{
		Stat->ClearArmorData(ArmorData->ArmorType);
		return;
	}
}

const FInventoryItem UInventoryManager::GetItemByItemID(FName ItemID) const
{
	FInventoryItem Result;

	for (auto& Item : Inventory)
	{
		if (ItemID == Item.ItemID)
		{
			Result = Item;
		}
	}

	return Result;
}

TArray<FInventoryItem> UInventoryManager::GetItemsByType(EItemType Type)
{
	TArray<FInventoryItem> Result;

	for (auto& Item : Inventory)
	{
		const FItemData* Data = GetItemData(Item.ItemID);
		if (Data && Data->ItemType == Type)
		{
			Result.Add(Item);
		}
	}

	return Result;
}

const FItemData* UInventoryManager::GetItemData(FName ItemID) const
{
	if (!ItemDataTable) return nullptr;

	return ItemDataTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));
}

const FConsumableData* UInventoryManager::GetConsumableData(FName ItemID) const
{
	if (!ConsumableDataTable) return nullptr;

	return ConsumableDataTable->FindRow<FConsumableData>(ItemID, TEXT("GetConsumableData"));
}

const FWeaponData* UInventoryManager::GetWeaponData(FName ItemID) const
{
	if (!WeaponDataTable) return nullptr;

	return WeaponDataTable->FindRow<FWeaponData>(ItemID, TEXT("GetWeaponData"));
}

const FArmorData* UInventoryManager::GetArmorData(FName ItemID) const
{
	if (!ArmorDataTable) return nullptr;

	return ArmorDataTable->FindRow<FArmorData>(ItemID, TEXT("GetArmorData"));
}

TMap<EEquipmentSlotType, FInventoryItem> UInventoryManager::GetAllHotkeyItems() const
{
	TMap<EEquipmentSlotType, FInventoryItem> Result;

	for (const auto& Pair : EquippedItems.EquippedItemMap)
	{
		if (Pair.Key >= EEquipmentSlotType::Hotkey_1)
		{
			Result.Add(Pair);
		}
	}

	return Result;
}

void UInventoryManager::SaveInventoryData()
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !GI->CurrentSave) return;

	GI->CurrentSave->InventoryItems = Inventory;
}

void UInventoryManager::LoadInventoryData()
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !GI->CurrentSave) return;

	Inventory = GI->CurrentSave->InventoryItems;
}

void UInventoryManager::SaveEquipmentData(FInventoryItem Item, EEquipmentSlotType SlotType)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !GI->CurrentSave) return;

	bool bHasChanged = false;

	switch (SlotType)
	{
	case EEquipmentSlotType::Weapon:
		GI->CurrentSave->EquipmentData.Weapon = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Head:
		GI->CurrentSave->EquipmentData.Head = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Body:
		GI->CurrentSave->EquipmentData.Body = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Glove:
		GI->CurrentSave->EquipmentData.Glove = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Leg:
		GI->CurrentSave->EquipmentData.Leg = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Foot:
		GI->CurrentSave->EquipmentData.Foot = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_1:
		GI->CurrentSave->EquipmentData.Hotkey_1 = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_2:
		GI->CurrentSave->EquipmentData.Hotkey_2 = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_3:
		GI->CurrentSave->EquipmentData.Hotkey_3 = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_4:
		GI->CurrentSave->EquipmentData.Hotkey_4 = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_5:
		GI->CurrentSave->EquipmentData.Hotkey_5 = Item;
		bHasChanged = true;
		break;
	case EEquipmentSlotType::Hotkey_6:
		GI->CurrentSave->EquipmentData.Hotkey_6 = Item;
		bHasChanged = true;
		break;
	}

	if (bHasChanged)
	{
		GI->MarkDirty();
	}
}

void UInventoryManager::LoadEquipmentData()
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (!GI || !GI->CurrentSave) return;

	FEquipmentData Data = GI->CurrentSave->EquipmentData;

	GI->CurrentSave->bHasHotkeyItems = false;

	if (Data.Weapon.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Weapon, Data.Weapon);
	}
	if (Data.Head.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Head, Data.Head);
	}
	if (Data.Body.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Body, Data.Body);
	}
	if (Data.Glove.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Glove, Data.Glove);
	}
	if (Data.Leg.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Leg, Data.Leg);
	}
	if (Data.Foot.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Foot, Data.Foot);
	}
	if (Data.Hotkey_1.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_1, Data.Hotkey_1);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
	if (Data.Hotkey_2.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_2, Data.Hotkey_2);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
	if (Data.Hotkey_3.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_3, Data.Hotkey_3);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
	if (Data.Hotkey_4.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_4, Data.Hotkey_4);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
	if (Data.Hotkey_5.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_5, Data.Hotkey_5);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
	if (Data.Hotkey_6.ItemID != NAME_None)
	{
		EquippedItems.EquippedItemMap.Add(EEquipmentSlotType::Hotkey_6, Data.Hotkey_6);
		GI->CurrentSave->bHasHotkeyItems = true;
	}
}
