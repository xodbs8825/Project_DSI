// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentDetailWidget.generated.h"

struct FInventoryItem;
struct FItemData;
struct FWeaponData;
struct FArmorData;

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_DSI_API UEquipmentDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowItemDetails(const FInventoryItem& Item, const FItemData& ItemData);

	void SetWeaponStatText(const FWeaponData& Weapon);
	void SetArmorStatText(const FArmorData& Armor);

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemStatText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatTypeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescriptionText;
};
