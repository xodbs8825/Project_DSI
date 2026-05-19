// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Character) return EBTNodeResult::Failed;

	auto Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (nullptr == Target) return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - Character->GetActorLocation();
	LookVector.Z = 0.0f;

	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	Character->SetActorRotation(FMath::RInterpTo(Character->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
