// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathTriggered);

UCLASS()
class PROJECT_DSI_API UDeathWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnDeathTriggered OnDeathTriggered;
	
};
