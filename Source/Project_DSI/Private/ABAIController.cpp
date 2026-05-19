// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));
const FName AABAIController::IsDeadKey(TEXT("IsDead"));
const FName AABAIController::ResetKey(TEXT("Reset"));
const FName AABAIController::EnemyStateKey(TEXT("State"));

AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/AI/BB_TestCharacter.BB_TestCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/AI/BT_TestCharacter.BT_TestCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* blackBoard = Blackboard;
	if (UseBlackboard(BBAsset, blackBoard))
	{
		blackBoard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		Blackboard = blackBoard;
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}
