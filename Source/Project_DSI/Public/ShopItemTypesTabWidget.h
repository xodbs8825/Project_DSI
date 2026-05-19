// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemTypesTabWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UShopItemTypesTabWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ButtonClicked();

public:
	void SetItemType(EItemType Type);
	void SetIsFocused(bool bValue);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemTypeText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemTypeButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, UTexture2D*> TextureMap;

	EItemType TabItemType;

	bool IsFocused = false;
};
