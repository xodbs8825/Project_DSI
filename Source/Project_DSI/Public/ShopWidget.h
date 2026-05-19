// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UShopItemDetailWidget;
class UShopItemSlotWidget;

struct FItemData;
struct FShopItem;

enum class EShopItemFocusState : uint8
{
	None		UMETA(DisplayName = "None"),
	Hovered		UMETA(DisplayName = "Hovered"),
	Selected	UMETA(DisplayName = "Selected")
};

UCLASS()
class PROJECT_DSI_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetShopItems(const TArray<FShopItem>& InItems);

	bool IsSlotSelected(const UShopItemSlotWidget* Slot) const;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleHovered(UShopItemSlotWidget* HoveredSlot);

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandleClicked(UShopItemSlotWidget* ClickedSlot);

	UFUNCTION()
	void HandleHoldProgress(float Ratio, UShopItemSlotWidget* PressedSlot);

	UFUNCTION()
	void HandleHoldCompleted(UShopItemSlotWidget* ItemSlot);

	UFUNCTION()
	void PurchaseCountUp();

	UFUNCTION()
	void PurchaseCountDown();

	UFUNCTION()
	void ItemTypeChanged(EItemType NewType);

	void PurchaseItem(const FShopItem& Item);

	void LoadShopItems(const TArray<FShopItem>& InItems, EItemType InType, class UInventoryManager* Inventory);

	//static const FItemDataStruct* GetItemDataStruct(const FShopItem& ShopItem);

	int32 GetMaxPurchaseCount();

protected:
	UPROPERTY()
	int32 ColumnCount = 9;

	UPROPERTY()
	int32 CurrentRowCount = 4;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ItemListPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UPanelWidget* ItemTypesPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UShopItemSlotWidget> ItmeSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UItemTypeTabWidget> ItemTypeTabClass;

	UPROPERTY(meta = (BindWidget))
	UShopItemDetailWidget* DetailWidget;

	FShopItem* FocusedItem;
	EShopItemFocusState FocusState = EShopItemFocusState::None;

	TArray<FShopItem> CurrentShopItems = TArray<FShopItem>();

	UShopItemSlotWidget* SelectedSlot;

	int32 CurrentPurchaseCount = 1;

	EItemType CurrentItemType;

	TMap<EItemType, UItemTypeTabWidget*> ItemTypeMap;
};
