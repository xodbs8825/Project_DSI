// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatDetailWidget.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "Components/TextBlock.h"
#include "DSI_SaveGame.h"
#include "BaseGameInstance.h"

void UPlayerStatDetailWidget::SetStatDetail()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UABCharacterStatComponent* Stats = Player->CharacterStatManager;
	if (!Stats) return;

	FString Name = FString::Printf(TEXT(""));

	if (UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>())
	{
		if (GI->CurrentSave)
		{
			Name = GI->CurrentSave->CharacterName;
		}
	}

	CharacterNameText->SetText(FText::FromString(Name));

	LevelText->SetText(FText::AsNumber(Stats->Level));

	VigorText->SetText(FText::AsNumber(Stats->PlayerStat.Vigor));
	EnduranceText->SetText(FText::AsNumber(Stats->PlayerStat.Endurance));
	StrengthText->SetText(FText::AsNumber(Stats->PlayerStat.Strength));

	CurrentHPText->SetText(FText::AsNumber(Stats->CurrentHP));
	MaxHPText->SetText(FText::AsNumber(Stats->MaxHP));
	CurrentStaminaText->SetText(FText::AsNumber(Stats->CurrentStamina));
	MaxStaminaText->SetText(FText::AsNumber(Stats->MaxStamina));

	DamageText->SetText(FText::AsNumber(Stats->PlayerDamage));
	AddedDamageText->SetText(FText::AsNumber(Stats->GetWeaponDamage()));
	DefenceText->SetText(FText::AsNumber(Stats->Defence));
	AddedDefenceText->SetText(FText::AsNumber(Stats->GetOverallArmorDefence()));

	FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));

	VigorText->SetColorAndOpacity(White);
	EnduranceText->SetColorAndOpacity(White);
	StrengthText->SetColorAndOpacity(White);

	CurrentHPText->SetColorAndOpacity(White);
	MaxHPText->SetColorAndOpacity(White);
	CurrentStaminaText->SetColorAndOpacity(White);
	MaxStaminaText->SetColorAndOpacity(White);

	DamageText->SetColorAndOpacity(White);
	AddedDamageText->SetColorAndOpacity(White);
	DefenceText->SetColorAndOpacity(White);
	AddedDefenceText->SetColorAndOpacity(White);
}

void UPlayerStatDetailWidget::SetVigor(int32 Level, int32 Vigor, bool HasStatChanged)
{
	LevelText->SetText(FText::AsNumber(Level));
	VigorText->SetText(FText::AsNumber(Vigor));

	FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));
	FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#DADA00FF")));

	VigorText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UABCharacterStatComponent* Stats = Player->CharacterStatManager;
	if (!Stats) return;

	LevelText->SetColorAndOpacity(Stats->Level != Level ? Yellow : White);

	float HP = Stats->ApplySoftCap(Vigor, Stats->VigorSoftCaps) + 400.0f;
	MaxHPText->SetText(FText::AsNumber(HP));
	MaxHPText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	float Defence = Stats->CalculateDefence(Level, Stats->PlayerStat.Strength);
	DefenceText->SetText(FText::AsNumber(Defence));
	DefenceText->SetColorAndOpacity(HasStatChanged ? Yellow : White);
}

void UPlayerStatDetailWidget::SetEndurance(int32 Level, int32 Endurance, bool HasStatChanged)
{
	LevelText->SetText(FText::AsNumber(Level));
	EnduranceText->SetText(FText::AsNumber(Endurance));

	FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));
	FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#DADA00FF")));

	EnduranceText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UABCharacterStatComponent* Stats = Player->CharacterStatManager;
	if (!Stats) return;

	LevelText->SetColorAndOpacity(Stats->Level != Level ? Yellow : White);

	float Stamina = Stats->ApplySoftCap(Endurance, Stats->EnduranceSoftCaps) + 100.0f;
	MaxStaminaText->SetText(FText::AsNumber(Stamina));
	MaxStaminaText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	float Defence = Stats->CalculateDefence(Level, Stats->PlayerStat.Strength);
	DefenceText->SetText(FText::AsNumber(Defence));
	DefenceText->SetColorAndOpacity(HasStatChanged ? Yellow : White);
}

void UPlayerStatDetailWidget::SetStrength(int32 Level, int32 Strength, bool HasStatChanged)
{
	LevelText->SetText(FText::AsNumber(Level));
	StrengthText->SetText(FText::AsNumber(Strength));

	FLinearColor White = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#FFFFFFFF")));
	FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor::FromHex(TEXT("#DADA00FF")));

	StrengthText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UABCharacterStatComponent* Stats = Player->CharacterStatManager;
	if (!Stats) return;

	LevelText->SetColorAndOpacity(Stats->Level != Level ? Yellow : White);

	float Damage = Stats->ApplySoftCap(Strength, Stats->StrengthSoftCaps) + 50.0f;
	DamageText->SetText(FText::AsNumber(Damage));
	DamageText->SetColorAndOpacity(HasStatChanged ? Yellow : White);

	float Defence = Stats->CalculateDefence(Level, Strength);
	DefenceText->SetText(FText::AsNumber(Defence));
	DefenceText->SetColorAndOpacity(HasStatChanged ? Yellow : White);
}
