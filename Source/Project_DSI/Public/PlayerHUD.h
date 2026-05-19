// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UProgressBar;
class USizeBox;

class UHUDHotkeySlotWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShowHUD);

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void RefreshHotkey();
	void NextHotkey(UInventoryManager* Inventory);
	void PrevHotkey(UInventoryManager* Inventory);
	void ClearAllHotkeySlots();

	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION()
	void UpdateStamina(float CurrentStamina, float MaxStamina);

	void UpdateSouls(int32 Souls);
	void UpdateDroppesSouls(int32 DroppedSouls, bool IsSoulsDropped);

	UFUNCTION()
	void CreateItemEntry(const FItemData& ItemData, int32 Count);

	//UFUNCTION()
	//void SetHPStaminaSize(float CurrentHP, float VigorMaxHP, float CurrentStamina, float EnduranceMaxStamina);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	//UPROPERTY(meta = (BindWidget))
	//USizeBox* HPSizeBox;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	//UPROPERTY(meta = (BindWidget))
	//USizeBox* StaminaSizeBox;

	UPROPERTY(meta = (BindWidget))
	UHUDHotkeySlotWidget* Hotkey_1;

	UPROPERTY(meta = (BindWidget))
	UHUDHotkeySlotWidget* Hotkey_2;

	UPROPERTY(meta = (BindWidget))
	UHUDHotkeySlotWidget* Hotkey_3;

	UPROPERTY(meta = (BindWidget))
	UHUDHotkeySlotWidget* Hotkey_4;

	UPROPERTY(meta = (BindWidget))
	UHUDHotkeySlotWidget* Hotkey_5;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentSoulsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DroppedSoulsText;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ItemEntryPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UItemEntryWidget> ItemEntryWidgetClass;

public:
	UPROPERTY(BlueprintReadOnly)
	EEquipmentSlotType CurrentType;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHotkeyIndex = 0;

	UPROPERTY(BlueprintAssignable)
	FOnHideHUD OnHideHUD;

	UPROPERTY(BlueprintAssignable)
	FOnShowHUD OnShowHUD;
};
