// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "GameFramework/SaveGame.h"
#include "DSI_SaveGame.generated.h"

USTRUCT()
struct FDroppedItemData
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	int32 Count;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FGuid DropID;

	bool operator==(const FDroppedItemData& Other) const
	{
		return DropID == Other.DropID;
	}
};

USTRUCT()
struct FEquipmentData
{
	GENERATED_BODY()

	UPROPERTY()
	FInventoryItem Weapon;

	UPROPERTY()
	FInventoryItem Head;

	UPROPERTY()
	FInventoryItem Body;

	UPROPERTY()
	FInventoryItem Glove;

	UPROPERTY()
	FInventoryItem Leg;

	UPROPERTY()
	FInventoryItem Foot;

	UPROPERTY()
	FInventoryItem Hotkey_1;

	UPROPERTY()
	FInventoryItem Hotkey_2;

	UPROPERTY()
	FInventoryItem Hotkey_3;

	UPROPERTY()
	FInventoryItem Hotkey_4;

	UPROPERTY()
	FInventoryItem Hotkey_5;

	UPROPERTY()
	FInventoryItem Hotkey_6;
};

UCLASS()
class PROJECT_DSI_API UDSI_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 CharacterID = -1;

	UPROPERTY()
	FString CharacterName;

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	float PlayTime = 0.0f;

	UPROPERTY()
	FName CurrentLevelName;




	UPROPERTY()
	FTransform PlayerTransform;

	UPROPERTY()
	bool bHasValidPlayerTransform;

	UPROPERTY()
	float HP;

	UPROPERTY()
	int32 Souls;

	UPROPERTY()
	FName LastBonfireID;

	UPROPERTY()
	TArray<FName> DeadEnemies;

	UPROPERTY()
	TArray<FName> ActivatedBonfires;

	UPROPERTY()
	TArray<FName> CollectedWorldItems;

	UPROPERTY()
	TArray<FDroppedItemData> DroppedItems;

	UPROPERTY()
	FTransform DroppedSoulsTransform;

	UPROPERTY()
	int32 DroppedSoulsAmount;

	UPROPERTY()
	int32 VigorLevel;

	UPROPERTY()
	int32 EnduranceLevel;

	UPROPERTY()
	int32 StrengthLevel;

	UPROPERTY()
	TArray<FInventoryItem> InventoryItems;

	UPROPERTY()
	FEquipmentData EquipmentData;

	UPROPERTY()
	bool bHasHotkeyItems;

	UPROPERTY()
	bool bHasSaveNewlyCreated;
};
