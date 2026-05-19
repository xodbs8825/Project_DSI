// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetIndicatorWidget.h"
#include "Components/Image.h"
#include "ABEnemyCharacter.h"

void UTargetIndicatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();



	//OnUpdateIndicatorColor.AddDynamic(this, &UTargetIndicatorWidget::UpdateIndicatorColor);
}

void UTargetIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OwnerEnemyCharacter)
	{
		UpdateIndicatorColor();
	}
}

void UTargetIndicatorWidget::UpdateIndicatorColor()
{
	check(IndicatorImage);

	if (OwnerEnemyCharacter->IsBeingExecuted)
	{
		IndicatorImage->SetColorAndOpacity(FColor::White);
	}
	else
	{
		if (OwnerEnemyCharacter->IsPlayerCharacterBehind || OwnerEnemyCharacter->IsKnockedDown)
		{
			IndicatorImage->SetColorAndOpacity(FColor::Red);
		}
		else
		{
			IndicatorImage->SetColorAndOpacity(FColor::White);
		}
	}
}

void UTargetIndicatorWidget::SetOwnerEnemyCharacter(AABEnemyCharacter* InEnemyCharacter)
{
	OwnerEnemyCharacter = InEnemyCharacter;
}
