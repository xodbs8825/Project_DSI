// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItemEffect.generated.h"

UCLASS()
class PROJECT_DSI_API ABaseItemEffect : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseItemEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyEffect(class ABaseCharacter* Player, int32 Count);
};
