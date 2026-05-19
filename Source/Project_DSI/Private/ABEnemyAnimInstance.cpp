// Fill out your copyright notice in the Description page of Project Settings.


#include "ABEnemyAnimInstance.h"
#include "ABEnemyCharacter.h"
#include "TargetIndicatorWidget.h"
#include "Components/WidgetComponent.h"

UABEnemyAnimInstance::UABEnemyAnimInstance()
{
}

void UABEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (::IsValid(SourceCharacter))
	{
		CurrentPawnSpeed = SourceCharacter->GetVelocity().Size();
		IsInAir = SourceCharacter->GetMovementComponent()->IsFalling();
		//IsDead = SourceCharacter->GetIsDead();
	}
}

void UABEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if (SourceCharacter) TargetWidget = Cast<UTargetIndicatorWidget>(SourceCharacter->TargetIndicatorWidgetComponent->GetUserWidgetObject());

}

void UABEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto Pawn = TryGetPawnOwner();
	SourceCharacter = Cast<AABEnemyCharacter>(Pawn);

}

void UABEnemyAnimInstance::ResetAnimInstance()
{
	StopAllMontages(0.0f);
	IsDead = false;
}

void UABEnemyAnimInstance::AnimNotify_KnockDown_Start()
{
	SourceCharacter->IsKnockedDown = true;
}

void UABEnemyAnimInstance::AnimNotify_KnockDown_End()
{
	SourceCharacter->IsKnockedDown = false;
	SourceCharacter->IsBeingKnockedDown = false;
	SourceCharacter->IsGettingUp = true;
}

void UABEnemyAnimInstance::AnimNotify_CheckIsDead()
{
	if (!SourceCharacter->GetIsDead())
	{
		IsAbleToJumpToNextGetUpSection = true;
	}
}

void UABEnemyAnimInstance::AnimNotify_JumpToNextGetUpSection()
{
	if (IsAbleToJumpToNextGetUpSection)
	{
		Montage_JumpToSection(TEXT("GetUp"));
		SourceCharacter->IsKnockedDown = false;
		SourceCharacter->IsBeingKnockedDown = false;
		SourceCharacter->IsGettingUp = true;
	}
	else
	{
		IsDead = true;
		SourceCharacter->SetActorEnableCollision(false);
	}
}

void UABEnemyAnimInstance::AnimNotify_SetIsDead()
{
	IsDead = true;
	SourceCharacter->SetActorEnableCollision(false);
}
