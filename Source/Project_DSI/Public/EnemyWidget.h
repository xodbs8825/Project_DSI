// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPBarUpdated, float, Ratio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostureBarUpdated, float, Ratio);

class UProgressBar;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitEnemyWidget(class AABEnemyCharacter* InEnemy);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void UpdateHPBar(float Ratio);

	UFUNCTION()
	void UpdatePostureBar(float Ratio);

public:
	FOnHPBarUpdated OnHPBarUpdated;
	FOnPostureBarUpdated OnPostureBarUpdated;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PostureBar;

	class AABEnemyCharacter* EnemyCharacter;
};
