// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "ItemTypeTabWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemTypeChanged, EItemType, NewType);

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UItemTypeTabWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetItemType(EItemType Type);
	void SetIsFocused(bool bValue);

	UFUNCTION()
	void ButtonClicked();

public:
	FOnItemTypeChanged OnItemTypeChanged;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemTypeButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemTypeText;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditAnywhere)
	EItemType TabItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EItemType, UTexture2D*> ItemTypeTextureMap;

	bool IsFocused = false;
};
