// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UBonfireWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetBonfireName(FName Name);

protected:
	UFUNCTION()
	void RestAtBonfire();

	UFUNCTION()
	void OnLevelUpButtonPressed();

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* RestButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LevelUpButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BonfireName;
};
