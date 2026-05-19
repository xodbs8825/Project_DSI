// Fill out your copyright notice in the Description page of Project Settings.


#include "BonfireWidget.h"
#include "BonfireSubsystem.h"
#include "ABPlayerController.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBonfireWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RestButton->OnClicked.AddDynamic(this, &UBonfireWidget::RestAtBonfire);
	LevelUpButton->OnClicked.AddDynamic(this, &UBonfireWidget::OnLevelUpButtonPressed);
}

void UBonfireWidget::SetBonfireName(FName Name)
{
	BonfireName->SetText(FText::FromName(Name));
}

void UBonfireWidget::RestAtBonfire()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBonfireSubsystem* Sub = GI->GetSubsystem<UBonfireSubsystem>())
		{
			Sub->ResetEnemies();
		}
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Player->CharacterStatManager->RecoverFull();
}

void UBonfireWidget::OnLevelUpButtonPressed()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABPC->OpenLevelUpUI();
}
