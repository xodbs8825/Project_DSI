// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterWidget.h"
#include "BaseCharacter.h"
#include "Components/ProgressBar.h"

void UBaseCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

ESlateVisibility UBaseCharacterWidget::BindHPBarVisibility()
{
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwningPlayerPawn()))
	{
		//Character->OnTargetChanged.AddLambda([this]() -> void
		//	{
		//		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("0"));
		//		ReturnValue = SetHPBarVisibility();
		//	});

		return ReturnValue;
	}

	return ESlateVisibility::Hidden;
}

ESlateVisibility UBaseCharacterWidget::SetHPBarVisibility()
{
	return ESlateVisibility::Hidden;
}

ESlateVisibility UBaseCharacterWidget::Test()
{
	FPlatformProcess::Sleep(1.0f);

	return ESlateVisibility();
}
