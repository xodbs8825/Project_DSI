// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemTypesTabWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UShopItemTypesTabWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(ItemTypeButton)

	ItemTypeButton->OnClicked.AddDynamic(this, &UShopItemTypesTabWidget::ButtonClicked);
}

void UShopItemTypesTabWidget::SetItemType(EItemType Type)
{
	TabItemType = Type;

	ItemTypeText->SetText(StaticEnum<EItemType>()->GetDisplayNameTextByValue(static_cast<int64>(TabItemType)));

	UTexture2D* CurrentIcon = TextureMap.FindChecked(TabItemType);
	IconImage->SetBrushFromTexture(CurrentIcon);
}

void UShopItemTypesTabWidget::SetIsFocused(bool bValue)
{
	IsFocused = bValue;

	if (IsFocused)
	{
		FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#DADA00FF")));
		IconImage->SetColorAndOpacity(Yellow);

		ItemTypeText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));
		IconImage->SetColorAndOpacity(White);

		ItemTypeText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShopItemTypesTabWidget::ButtonClicked()
{
	SetIsFocused(true);
}
