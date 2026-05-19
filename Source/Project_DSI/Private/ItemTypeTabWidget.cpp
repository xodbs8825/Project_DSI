// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemTypeTabWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UItemTypeTabWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetItemType(TabItemType);

	ItemTypeButton->OnReleased.AddDynamic(this, &UItemTypeTabWidget::ButtonClicked);
}

void UItemTypeTabWidget::SetItemType(EItemType Type)
{
	TabItemType = Type;

	ItemTypeText->SetText(StaticEnum<EItemType>()->GetDisplayNameTextByValue(static_cast<int64>(TabItemType)));

	UTexture2D* CurrentIcon = ItemTypeTextureMap.FindChecked(TabItemType);
	IconImage->SetBrushFromTexture(CurrentIcon);
}

void UItemTypeTabWidget::SetIsFocused(bool bValue)
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

void UItemTypeTabWidget::ButtonClicked()
{
	OnItemTypeChanged.Broadcast(TabItemType);

	SetIsFocused(true);
}
