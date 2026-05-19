// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "SaveSubsystem.h"
#include "CharacterSaveSlotWidget.h"
#include "Components/PanelWidget.h"
#include "Components/EditableText.h"
#include "FadeWidget.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NewGameButton->OnClicked.AddDynamic(this, &UTitleWidget::OnNewGameClicked);
	ContinueButton->OnClicked.AddDynamic(this, &UTitleWidget::OnContinueClicked);
	LoadGameButton->OnClicked.AddDynamic(this, &UTitleWidget::OnLoadGameClicked);

	ConfirmButton->OnClicked.AddDynamic(this, &UTitleWidget::OnConfirmClicked);
	CancelButton->OnClicked.AddDynamic(this, &UTitleWidget::OnCancelClicked);

	CharacterNameEditText->OnTextChanged.AddDynamic(this, &UTitleWidget::OnCharacterNameChanged);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (USaveSubsystem* Sub = GI->GetSubsystem<USaveSubsystem>())
		{
			if (!Sub->DoesCharacterSlotsExist())
			{
				ContinueButton->SetVisibility(ESlateVisibility::Hidden);
				LoadGameButton->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	CharacterCreatePrompt->SetVisibility(ESlateVisibility::Hidden);
	CharacterSlotPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UTitleWidget::FadeOutScreen()
{
	FadeWidget->OnFadeOut.Broadcast();
}

void UTitleWidget::OnNewGameClicked()
{
	CharacterCreatePrompt->SetVisibility(ESlateVisibility::Visible);
}

void UTitleWidget::OnContinueClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (USaveSubsystem* Sub = GI->GetSubsystem<USaveSubsystem>())
		{
			Sub->ContinueGame();

			FadeOutScreen();
		}
	}
}

void UTitleWidget::OnLoadGameClicked()
{
	if (!CharacterSaveSlotClass || !SaveSlotPanel) return;

	CharacterSlotPanel->SetVisibility(ESlateVisibility::Visible);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (USaveSubsystem* Sub = GI->GetSubsystem<USaveSubsystem>())
		{
			for (auto& CharacterSlot : Sub->GetCharacterSlots())
			{
				UCharacterSaveSlotWidget* SaveSlot = CreateWidget<UCharacterSaveSlotWidget>(this, CharacterSaveSlotClass);
				if (!SaveSlot) continue;

				SaveSlot->SetSlotDetail(CharacterSlot, this);

				SaveSlotPanel->AddChild(SaveSlot);
			}
		}
	}
}

void UTitleWidget::OnConfirmClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (USaveSubsystem* Sub = GI->GetSubsystem<USaveSubsystem>())
		{
			Sub->CreateCharacter(CharacterNameEditText->GetText().ToString());

			FadeOutScreen();
		}
	}
}

void UTitleWidget::OnCancelClicked()
{
	CharacterCreatePrompt->SetVisibility(ESlateVisibility::Hidden);
}

void UTitleWidget::OnCharacterNameChanged(const FText& Text)
{
	FString Input = Text.ToString();

	const int32 MaxLength = 12;

	if (Input.Len() > MaxLength)
	{
		Input = Input.Left(MaxLength);

		CharacterNameEditText->SetText(FText::FromString(Input));
	}
}
