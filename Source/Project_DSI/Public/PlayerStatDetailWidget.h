// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatDetailWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_DSI_API UPlayerStatDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetStatDetail();

	void SetVigor(int32 Level, int32 Vigor, bool HasStatChanged);
	void SetEndurance(int32 Level, int32 Endurance, bool HasStatChanged);
	void SetStrength(int32 Level, int32 Strength, bool HasStatChanged);

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* VigorText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnduranceText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* StrengthText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHPText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHPText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentStaminaText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxStaminaText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AddedDamageText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefenceText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AddedDefenceText;
};
