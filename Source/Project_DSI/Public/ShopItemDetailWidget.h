// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemDetailWidget.generated.h"

struct FShopItem;
struct FItemData;

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchaseQuantityUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchaseQuantityDown);

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UShopItemDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowItem(const FShopItem& Item, const FItemData& ItemData);
	void Hide();

	void SetPurchaseCountText(int32 Count);
	void ShowPurchaseCount();
	void HidePurchaseCount();

	void SetItemQuantityVisibility(ESlateVisibility NewVisibility);

	void EnablePurchaseCounter(bool bValue);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnQuantityUpButtonClicked();

	UFUNCTION()
	void OnQuantityDownButtonClicked();

public:
	UPROPERTY(BlueprintAssignable)
	FOnPurchaseQuantityUp OnPurchaseQuantityUp;

	UPROPERTY(BlueprintAssignable)
	FOnPurchaseQuantityDown OnPurchaseQuantityDown;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemTypeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemMaxCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPrice;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* PurchaseCountContainer;

	UPROPERTY(meta = (BindWidget))
	UButton* QuantityDownBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* QuantityUpBtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PurchaseCount;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ItemQuantityContainer;
};
