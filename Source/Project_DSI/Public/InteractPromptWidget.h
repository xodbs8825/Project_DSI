// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractPromptWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPromptText(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptText;
};
