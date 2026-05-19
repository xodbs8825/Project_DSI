// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentViewWidget.generated.h"

class UEquipmentViewSlot;
class UEquipmentDetailWidget;
class UPlayerStatDetailWidget;

UCLASS()
class PROJECT_DSI_API UEquipmentViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void InitSlotMap();

	void LoadEquipment(class UInventoryManager* Inventory);

	UFUNCTION()
	void SetConsumableDetailWidget(FInventoryItem Item);

	UFUNCTION()
	void HideConsumableDetailWidget();

	UFUNCTION()
	void SetEquipmentDetailWidget(FInventoryItem Item);

	UFUNCTION()
	void HideEquipmentDetailWidget();

public:
	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* WeaponSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HeadSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* BodySlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* GloveSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* LegSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* FootSlot;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HotkeySlot_1;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HotkeySlot_2;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HotkeySlot_3;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HotkeySlot_4;

	UPROPERTY(meta = (BindWidget))
	UEquipmentViewSlot* HotkeySlot_5;

	//UPROPERTY(meta = (BindWidget))
	//UEquipmentViewSlot* HotkeySlot_6;

	UPROPERTY(meta = (BindWidget))
	UEquipmentDetailWidget* EquipmentDetailWidget;

	UPROPERTY(meta = (BindWidget))
	class UInventoryDetailWidget* ConsumableDetailWidget;

	UPROPERTY(meta = (BindWidget))
	UPlayerStatDetailWidget* PlayerStatDetailWidget;

	TMap<EEquipmentSlotType, UEquipmentViewSlot*> SlotMap;
};
