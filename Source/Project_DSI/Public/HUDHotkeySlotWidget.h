// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "HUDHotkeySlotWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_DSI_API UHUDHotkeySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSlot(class UPlayerHUD* HUD, FInventoryItem Item);
	void ClearSlot();

public:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ItemCountOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCount;
};
