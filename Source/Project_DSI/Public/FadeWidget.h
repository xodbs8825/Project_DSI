// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeIn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeOut);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathFadeOut);

UCLASS()
class PROJECT_DSI_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnFadeIn OnFadeIn;

	UPROPERTY(BlueprintAssignable)
	FOnFadeOut OnFadeOut;

	UPROPERTY(BlueprintAssignable)
	FOnDeathFadeOut OnDeathFadeOut;
};
