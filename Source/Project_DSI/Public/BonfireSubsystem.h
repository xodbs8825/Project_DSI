// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BonfireSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UBonfireSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterEnemy(class AABEnemyCharacter* Enemy);
	void ResetEnemies();

public:
	TArray<TWeakObjectPtr<AABEnemyCharacter>> Enemies;
};
