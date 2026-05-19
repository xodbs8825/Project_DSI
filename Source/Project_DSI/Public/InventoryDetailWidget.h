// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDetailWidget.generated.h"

struct FInventoryItem;
struct FItemData;

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_DSI_API UInventoryDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowItemDetails(const FInventoryItem& Item, const FItemData& ItemData);

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemMaxCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescriptionText;
};
