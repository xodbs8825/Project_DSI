// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpWidget.generated.h"

class UButton;
class UTextBlock;

class UPlayerStatDetailWidget;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API ULevelUpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnVigorDownClicked();

	UFUNCTION()
	void OnVigorUpClicked();

	UFUNCTION()
	void OnEnduranceDownClicked();

	UFUNCTION()
	void OnEnduranceUpClicked();

	UFUNCTION()
	void OnStrengthDownClicked();

	UFUNCTION()
	void OnStrengthUpClicked();

	UFUNCTION()
	void OnLevelUpClicked();

private:
	void UpdateLevelUpWidget();

	void SetStatUpButtonVisibility(bool bValue);

public:
	void LoadPlayerStats();

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* VigorDownButton;

	UPROPERTY(meta = (BindWidget))
	UButton* VigorUpButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EnduranceDownButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EnduranceUpButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StrengthDownButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StrengthUpButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VigorText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnduranceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StrengthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingSoulText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RequiredSoulText;

	UPROPERTY(meta = (BindWidget))
	UButton* LevelUpButton;

	UPROPERTY(meta = (BindWidget))
	UPlayerStatDetailWidget* PlayerStatDetail;

private:
	int32 Vigor = 0;
	int32 Endurance = 0;
	int32 Strength = 0;

	int32 Level = 0;

	int32 RemainingSoul = 0;
	int32 RequiredSoul = 0;

	bool HasVigorChanged = false;
};
