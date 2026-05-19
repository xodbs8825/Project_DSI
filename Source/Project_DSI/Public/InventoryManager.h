// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "ConsumableBaseItem.h"
#include "WeaponBaseItem.h"
#include "ArmorBaseItem.h"
#include "InventoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemObtained, const FItemData&, ItemData, int32, ItemCount);

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Weapon,
	Head,
	Body,
	Glove,
	Leg,
	Foot,

	Hotkey_1,
	Hotkey_2,
	Hotkey_3,
	Hotkey_4,
	Hotkey_5,
	Hotkey_6
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

public:
	// 아이템 ID만 저장. 데이터는 필요할 때 데이터 테이블에서 조회
	UPROPERTY(BlueprintReadOnly)
	FName ItemID = TEXT("");

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FEquippedItems
{
	GENERATED_BODY();

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<EEquipmentSlotType, FInventoryItem> EquippedItemMap;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_DSI_API UInventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddItem(FName ItemID, int32 Count);
	bool UseItem(FName ItemID, int32 Count);
	bool RemoveItem(FName ItemID, int32 Count);
	bool DropItem(FName ItemID, int32 Count);

	void EquipItem(EEquipmentSlotType SlotType, const FInventoryItem& Item);
	void RemoveItemFromAllSlots(const FInventoryItem& Item);
	bool IsItemEquipped(const FInventoryItem& Item) const;
	bool IsHotkeySlot(EEquipmentSlotType SlotType);
	void UpdateEquipment(const FInventoryItem& Item);
	void UpdateItemStat(FInventoryItem Item);
	void ClearItemStat(FInventoryItem Item);

	const FInventoryItem GetItemByItemID(FName ItemID) const;
	TArray<FInventoryItem> GetItemsByType(EItemType Type);

	const FItemData* GetItemData(FName ItemID) const;
	const FConsumableData* GetConsumableData(FName ItemID) const;
	const FWeaponData* GetWeaponData(FName ItemID) const;
	const FArmorData* GetArmorData(FName ItemID) const;

	const TMap<EEquipmentSlotType, FInventoryItem>& GetEquippedItems() const { return EquippedItems.EquippedItemMap; }
	TMap<EEquipmentSlotType, FInventoryItem> GetAllHotkeyItems() const;

protected:
	void SaveInventoryData();
	void LoadInventoryData();

	void SaveEquipmentData(FInventoryItem Item, EEquipmentSlotType SlotType);
	void LoadEquipmentData();

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FInventoryItem> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	UDataTable* ItemDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	UDataTable* ConsumableDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	UDataTable* WeaponDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	UDataTable* ArmorDataTable;

	UPROPERTY(BlueprintReadOnly)
	FEquippedItems EquippedItems;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AItemDropActor> DropActorClass;

	FOnItemObtained OnItemObtained;
};
