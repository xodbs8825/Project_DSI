// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "AIController.h"
#include "ABAIController.generated.h"

/**
 * 
 */
enum EnemyState
{
	Patrolling,
	ChasingPlayer,
	SearchingPlayer
};

UCLASS()
class PROJECT_DSI_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName IsDeadKey;
	static const FName ResetKey;
	static const FName EnemyStateKey;

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};
