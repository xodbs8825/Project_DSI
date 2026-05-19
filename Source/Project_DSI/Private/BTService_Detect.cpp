// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "ABAIController.h"
#include "BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	//Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	Interval = DeltaSeconds;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	//ABaseCharacter* ControllingCharacter = Cast<ABaseCharacter>(ControllingPawn);
	//if (nullptr != ControllingCharacter)
	//{
	//	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AABAIController::IsDeadKey, ControllingCharacter->IsDead);
	//	if (ControllingCharacter->IsDead)
	//	{
	//		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
	//		return;
	//	}
	//}

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;
	if (nullptr == World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			ABaseCharacter* Character = Cast<ABaseCharacter>(OverlapResult.GetActor());
			if (Character && Character->GetController()->IsPlayerController())
			{
				if (Character->IsDead)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
					OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AABAIController::EnemyStateKey, EnemyState::SearchingPlayer);
					return;
				}

				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, Character);
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AABAIController::EnemyStateKey, EnemyState::ChasingPlayer);
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, Character->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Character->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else
	{
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AABAIController::EnemyStateKey, EnemyState::Patrolling);
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

}