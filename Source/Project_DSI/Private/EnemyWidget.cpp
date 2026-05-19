// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"
#include "ABEnemyCharacter.h"
#include "ABEnemyStatComponent.h"
#include "Components/ProgressBar.h"

void UEnemyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(HPBar);
	check(PostureBar);

	OnHPBarUpdated.AddDynamic(this, &UEnemyWidget::UpdateHPBar);
	OnPostureBarUpdated.AddDynamic(this, &UEnemyWidget::UpdatePostureBar);
}

void UEnemyWidget::InitEnemyWidget(AABEnemyCharacter* InEnemy)
{
	EnemyCharacter = InEnemy;
}

void UEnemyWidget::UpdateHPBar(float Ratio)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("%s"), *FString::SanitizeFloat(Ratio)));
	HPBar->SetPercent(Ratio);
}

void UEnemyWidget::UpdatePostureBar(float Ratio)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("%s"), *FString::SanitizeFloat(Ratio)));
	PostureBar->SetPercent(Ratio);
}
