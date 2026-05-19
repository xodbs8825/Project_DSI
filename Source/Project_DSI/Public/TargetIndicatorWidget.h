// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetIndicatorWidget.generated.h"

class AABEnemyCharacter;

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UTargetIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateIndicatorColor();

public:
	void SetOwnerEnemyCharacter(AABEnemyCharacter* InEnemyCharacter);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IndicatorImage;

	AABEnemyCharacter* OwnerEnemyCharacter;
};
