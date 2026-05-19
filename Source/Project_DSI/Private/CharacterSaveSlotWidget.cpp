// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSaveSlotWidget.h"
#include "BaseGameInstance.h"
#include "SaveSubsystem.h"
#include "TitleWidget.h"
#include "FadeWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCharacterSaveSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotButton->OnClicked.AddDynamic(this, &UCharacterSaveSlotWidget::OnSlotButtonClicked);
}

void UCharacterSaveSlotWidget::SetSlotDetail(FCharacterSlotInfo Info, UTitleWidget* Owner)
{
	SlotInfo = Info;
	TitleWidget = Owner;

	CharacterNameText->SetText(FText::FromString(Info.CharacterName));
}

void UCharacterSaveSlotWidget::OnSlotButtonClicked()
{
	if (UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>())
	{
		if (USaveSubsystem* Sub = GI->GetSubsystem<USaveSubsystem>())
		{
			Sub->LoadCharacter(SlotInfo.CharacterID);

			if (TitleWidget)
			{
				TitleWidget->FadeOutScreen();
			}
		}
	}
}
