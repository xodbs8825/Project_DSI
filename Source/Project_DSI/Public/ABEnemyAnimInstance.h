// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UABEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABEnemyAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;

public:
	void ResetAnimInstance();

private:
	UFUNCTION()
	void AnimNotify_KnockDown_Start();

	UFUNCTION()
	void AnimNotify_KnockDown_End();

	UFUNCTION()
	void AnimNotify_CheckIsDead();

	UFUNCTION()
	void AnimNotify_JumpToNextGetUpSection();

	UFUNCTION()
	void AnimNotify_SetIsDead();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool IsAttackStarted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* Victim_BackStabMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* Victim_ExecutionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* KnockedDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death", Meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathMontage_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death", Meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathMontage_KnockDown;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	class AABEnemyCharacter* SourceCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsDead = false;

	class UTargetIndicatorWidget* TargetWidget;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsAbleToJumpToNextGetUpSection = false;
};
