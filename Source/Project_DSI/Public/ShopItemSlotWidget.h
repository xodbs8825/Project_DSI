// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MerchantNPC.h"
#include "ShopItemSlotWidget.generated.h"

class UImage;
class UProgressBar;
class UButton;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered, UShopItemSlotWidget*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUnhovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemClicked, UShopItemSlotWidget*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemHoldProgress, float, Ratio, UShopItemSlotWidget*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHoldCompleted, UShopItemSlotWidget*, Slot);

/**
 *
 */
UCLASS()
class PROJECT_DSI_API UShopItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(const FShopItem& InShopItem, const FItemData& InItemData);

	void ClearSlot();

	UPROPERTY(BlueprintAssignable)
	FOnItemHovered OnItemHovered;

	UPROPERTY(BlueprintAssignable)
	FOnItemUnhovered OnItemUnhovered;

	UPROPERTY(BlueprintAssignable)
	FOnItemClicked OnItemClicked;

	UPROPERTY(BlueprintAssignable)
	FOnItemHoldProgress OnItemHoldProgress;

	UPROPERTY(BlueprintAssignable)
	FOnItemHoldCompleted OnItemHoldCompleted;

	void UpdateHoldProgressBar(float Ratio);
	void CancelHold();

	FShopItem* GetShopItem() { return &ShopItem; }
	FItemData GetItemData() { return ItemData; }

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ButtonHovered();

	UFUNCTION()
	void ButtonUnhovered();

	UFUNCTION()
	void ButtonClicked();

	UFUNCTION()
	void ButtonPressed();

	UFUNCTION()
	void ButtonReleased();

	void SetSelected(bool bSelected);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HoldProgressBar;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta= (BindWidget))
	class UImage* Frame;

	FShopItem ShopItem;
	FItemData ItemData;

	bool bIsSelected = false;
	bool bHolding = false;
	bool bHoldCompleted = false;

	float HoldElapsed = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hold")
	float RequiredHoldTime = 1.5f;
};
