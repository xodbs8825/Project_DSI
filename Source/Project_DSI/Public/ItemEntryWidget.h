// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UItemEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetItemEntry(const struct FItemData& ItemData, int32 Count);

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCount;
};
