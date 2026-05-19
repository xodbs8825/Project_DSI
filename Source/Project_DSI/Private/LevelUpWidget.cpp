// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUpWidget.h"
#include "ABPlayerController.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "PlayerStatDetailWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void ULevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VigorDownButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnVigorDownClicked);
	VigorUpButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnVigorUpClicked);

	EnduranceDownButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnEnduranceDownClicked);
	EnduranceUpButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnEnduranceUpClicked);

	StrengthDownButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnStrengthDownClicked);
	StrengthUpButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnStrengthUpClicked);

	LevelUpButton->OnClicked.AddDynamic(this, &ULevelUpWidget::OnLevelUpClicked);

	LoadPlayerStats();
}

void ULevelUpWidget::OnVigorDownClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Vigor--;
	VigorText->SetText(FText::AsNumber(Vigor));

	Level--;
	LevelText->SetText(FText::AsNumber(Level));

	RequiredSoul = Level * 100;
	RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

	RemainingSoul += RequiredSoul;
	RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

	if (Vigor == Player->CharacterStatManager->PlayerStat.Vigor)
	{
		LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
		VigorDownButton->SetVisibility(ESlateVisibility::Hidden);

		PlayerStatDetail->SetVigor(Level, Vigor, false);
		return;
	}

	if (RemainingSoul >= RequiredSoul)
	{
		SetStatUpButtonVisibility(true);
	}

	PlayerStatDetail->SetVigor(Level, Vigor, true);
}

void ULevelUpWidget::OnVigorUpClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	if (RemainingSoul > RequiredSoul)
	{
		Vigor++;
		VigorText->SetText(FText::AsNumber(Vigor));

		RequiredSoul = Level * 100;

		Level++;
		LevelText->SetText(FText::AsNumber(Level));

		RemainingSoul -= RequiredSoul;
		RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

		RequiredSoul = Level * 100;
		RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

		if (Vigor > Player->CharacterStatManager->PlayerStat.Vigor)
		{
			LevelUpButton->SetVisibility(ESlateVisibility::Visible);
			VigorDownButton->SetVisibility(ESlateVisibility::Visible);

			PlayerStatDetail->SetVigor(Level, Vigor, true);
		}
	}

	if (RemainingSoul < RequiredSoul)
	{
		SetStatUpButtonVisibility(false);
	}
}

void ULevelUpWidget::OnEnduranceDownClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Endurance--;
	EnduranceText->SetText(FText::AsNumber(Endurance));

	Level--;
	LevelText->SetText(FText::AsNumber(Level));

	RequiredSoul = Level * 100;
	RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

	RemainingSoul += RequiredSoul;
	RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

	if (Endurance == Player->CharacterStatManager->PlayerStat.Endurance)
	{
		LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
		EnduranceDownButton->SetVisibility(ESlateVisibility::Hidden);

		PlayerStatDetail->SetEndurance(Level, Endurance, false);
		return;
	}

	if (RemainingSoul >= RequiredSoul)
	{
		SetStatUpButtonVisibility(true);
	}

	PlayerStatDetail->SetEndurance(Level, Endurance, true);
}

void ULevelUpWidget::OnEnduranceUpClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	if (RemainingSoul > RequiredSoul)
	{
		Endurance++;
		EnduranceText->SetText(FText::AsNumber(Endurance));

		RequiredSoul = Level * 100;

		Level++;
		LevelText->SetText(FText::AsNumber(Level));

		RemainingSoul -= RequiredSoul;
		RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

		RequiredSoul = Level * 100;
		RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

		if (Endurance > Player->CharacterStatManager->PlayerStat.Endurance)
		{
			LevelUpButton->SetVisibility(ESlateVisibility::Visible);
			EnduranceDownButton->SetVisibility(ESlateVisibility::Visible);

			PlayerStatDetail->SetEndurance(Level, Endurance, true);
		}
	}

	if (RemainingSoul < RequiredSoul)
	{
		SetStatUpButtonVisibility(false);
	}
}

void ULevelUpWidget::OnStrengthDownClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Strength--;
	StrengthText->SetText(FText::AsNumber(Strength));

	Level--;
	LevelText->SetText(FText::AsNumber(Level));

	RequiredSoul = Level * 100;
	RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

	RemainingSoul += RequiredSoul;
	RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

	if (Strength == Player->CharacterStatManager->PlayerStat.Strength)
	{
		LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
		StrengthDownButton->SetVisibility(ESlateVisibility::Hidden);

		PlayerStatDetail->SetStrength(Level, Strength, false);
		return;
	}

	if (RemainingSoul >= RequiredSoul)
	{
		SetStatUpButtonVisibility(true);
	}

	PlayerStatDetail->SetStrength(Level, Strength, true);
}

void ULevelUpWidget::OnStrengthUpClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	if (RemainingSoul > RequiredSoul)
	{
		Strength++;
		StrengthText->SetText(FText::AsNumber(Strength));

		RequiredSoul = Level * 100;

		Level++;
		LevelText->SetText(FText::AsNumber(Level));

		RemainingSoul -= RequiredSoul;
		RemainingSoulText->SetText(FText::AsNumber(RemainingSoul));

		RequiredSoul = Level * 100;
		RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));

		if (Strength > Player->CharacterStatManager->PlayerStat.Strength)
		{
			LevelUpButton->SetVisibility(ESlateVisibility::Visible);
			StrengthDownButton->SetVisibility(ESlateVisibility::Visible);

			PlayerStatDetail->SetStrength(Level, Strength, true);
		}
	}

	if (RemainingSoul < RequiredSoul)
	{
		SetStatUpButtonVisibility(false);
	}
}

void ULevelUpWidget::OnLevelUpClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	FPlayerStatData PlayerStat = Player->CharacterStatManager->PlayerStat;

	int32 AddedVigor = Vigor - PlayerStat.Vigor;
	int32 AddedEndurance = Endurance - PlayerStat.Endurance;
	int32 AddedStrength = Strength - PlayerStat.Strength;

	const FPlayerStatData& Stats = FPlayerStatData(AddedVigor, AddedEndurance, AddedStrength);
	Player->CharacterStatManager->TryLevelUp(Stats);

	LoadPlayerStats();
}

void ULevelUpWidget::UpdateLevelUpWidget()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Vigor = Player->CharacterStatManager->PlayerStat.Vigor;
	Endurance = Player->CharacterStatManager->PlayerStat.Endurance;
	Strength = Player->CharacterStatManager->PlayerStat.Strength;
	RemainingSoul = Player->CharacterStatManager->Soul;
	RequiredSoul = Player->CharacterStatManager->GetLevelUpCost();
	Level = Player->CharacterStatManager->Level;

	VigorText->SetText(FText::AsNumber(Vigor));
	EnduranceText->SetText(FText::AsNumber(Endurance));
	StrengthText->SetText(FText::AsNumber(Strength));
	RemainingSoulText->SetText(FText::AsNumber((RemainingSoul)));
	RequiredSoulText->SetText(FText::AsNumber(RequiredSoul));
	LevelText->SetText(FText::AsNumber(Level));

	SetStatUpButtonVisibility(RemainingSoul >= RequiredSoul);

	VigorDownButton->SetVisibility(ESlateVisibility::Hidden);
	EnduranceDownButton->SetVisibility(ESlateVisibility::Hidden);
	StrengthDownButton->SetVisibility(ESlateVisibility::Hidden);

	LevelUpButton->SetVisibility(ESlateVisibility::Hidden);
}

void ULevelUpWidget::SetStatUpButtonVisibility(bool bValue)
{
	VigorUpButton->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	EnduranceUpButton->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	StrengthUpButton->SetVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ULevelUpWidget::LoadPlayerStats()
{
	UpdateLevelUpWidget();
	PlayerStatDetail->SetStatDetail();
}
