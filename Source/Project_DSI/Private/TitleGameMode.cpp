// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "Blueprint/UserWidget.h"

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (TitleWidgetClass)
	{
		UUserWidget* TitleWidget = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
		if (TitleWidget)
		{
			TitleWidget->AddToViewport();
		}

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}
