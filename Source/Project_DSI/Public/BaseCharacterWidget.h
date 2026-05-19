// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UBaseCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION(BlueprintCallable)
	ESlateVisibility BindHPBarVisibility();

	UFUNCTION(BlueprintCallable)
	ESlateVisibility SetHPBarVisibility();

	UFUNCTION(BlueprintCallable)
	ESlateVisibility Test();

private:
	FTimerHandle Timer;

	ESlateVisibility ReturnValue;
};
