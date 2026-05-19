// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UItemSlotWidget;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot1;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot2;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot3;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot4;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot5;

	UPROPERTY(meta = (BindWidget))
	UItemSlotWidget* Slot6;
};
