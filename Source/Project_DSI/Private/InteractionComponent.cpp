// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ScanTimer, this, &UInteractionComponent::FindInteractable, ScanInterval, true);
}

// BeginPlay에서 타이머 세팅
void UInteractionComponent::FindInteractable()
{
	// 컴포넌트가 플레이어 한테 있는지 확인
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled()) return;

	FVector PlayerLocation = OwnerPawn->GetActorLocation();

	// 상호작용이 가능한 액터가 이미 있는지 확인
	if (CurrentInteractActor)
	{
		// 상호작용이 가능한 액터가 상호작용이 가능한 거리보다 멀리 있으면 액터 지우기
		float DistSq = FVector::DistSquared(PlayerLocation, CurrentInteractActor->GetActorLocation());
		if (DistSq > TraceDistance * TraceDistance)
		{
			ClearCurrentTarget();
		}
	}

	// 액터 찾기
	FindNewInteractTarget();
}

void UInteractionComponent::Interact()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled() || bUIBlocking) return;

	if (!CurrentInteractActor) return;

	if (CurrentInteractActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		IInteractInterface::Execute_Interact(CurrentInteractActor, Cast<APlayerController>(GetOwner()->GetInstigatorController()));
	}
}

FText UInteractionComponent::GetCurrentInteractText() const
{
	if (!CurrentInteractActor) return FText::GetEmpty();

	if (CurrentInteractActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		return IInteractInterface::Execute_GetInteractText(CurrentInteractActor);
	}

	return FText::GetEmpty();
}

void UInteractionComponent::TryInteract()
{
	if (!CurrentInteractActor) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	if (CurrentInteractActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		IInteractInterface::Execute_Interact(CurrentInteractActor, PC);
	}
}

void UInteractionComponent::FindNewInteractTarget()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	FVector ViewLocation;
	FRotator ViewRotation;
	PC->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	FVector Start = ViewLocation;
	FVector End = Start + ViewRotation.Vector() * TraceDistance;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);

	bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(TraceDistance), Params);

	AActor* NewTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		// 상호작용 인터페이스가 있는지 확인
		if (!HitActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass())) continue;

		// 액터까지의 거리가 상호작용이 가능한 거리 안에 있으면 타겟 변경
		float DistSq = FVector::DistSquared(OwnerPawn->GetActorLocation(), HitActor->GetActorLocation());
		if (DistSq < BestDistSq && DistSq <= TraceDistance * TraceDistance)
		{
			BestDistSq = DistSq;
			NewTarget = HitActor;
		}
	}

	// 상호작용이 가능한 액터가 변경 되었으면 액터 레퍼런스 업데이트
	if (NewTarget != CurrentInteractActor)
	{
		CurrentInteractActor = NewTarget;
		OnTargetChanged.Broadcast(CurrentInteractActor);
	}
}

void UInteractionComponent::ClearCurrentTarget()
{
	CurrentInteractActor = nullptr;
	OnTargetChanged.Broadcast(nullptr);
}
