#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHeavyAttackHoldDelegate);
DECLARE_MULTICAST_DELEGATE(FOnReRollCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheck);
DECLARE_MULTICAST_DELEGATE(FOnEndAttack);
DECLARE_MULTICAST_DELEGATE(FOnCollisionOn);
DECLARE_MULTICAST_DELEGATE(FOnCollisionEnd);

DECLARE_DELEGATE(FOnExecutionDamage);
DECLARE_DELEGATE(FOnItemUsed);

UCLASS()
class PROJECT_DSI_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBaseAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	void PlayLightAttackMontage();
	void PlayHeavyAttackMontage();

	void SetHeavyAttackPlayRate(float PlayRate);

	void JumpToLightAttackMontageSection(int32 NewSection);
	void JumpToHeavyAttackMontageSection(int32 NewSection);

	void SetAxisValue(FVector2D MovementVector);
	void Reset();

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnHeavyAttackHoldDelegate OnHeavyAttackHold;
	FOnReRollCheckDelegate OnReRollCheck;
	FOnExecutionDamage OnExecutionDamage;

	FOnAttackHitCheck OnAttackHitCheck;
	FOnEndAttack OnEndAttack;
	FOnCollisionOn OnCollisionOn;
	FOnCollisionEnd OnCollisionEnd;

	FOnItemUsed OnItemUsed;

	void SetDeadAnim() { IsDead = true; }
	void SetSprintSpeed(float Speed);
	void SetIsSprinting(bool bValue) { IsSprinting = bValue; }
	void SetIsHoldingHeavyAttackKey(bool bValue) { IsHoldingHeavyAttackKey = bValue; }

public:
	bool IFrame;
	bool IsRolling;
	bool AbleToCheckReInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool JumpRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

private:
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	UFUNCTION()
	void AnimNotify_EndAttack();

	UFUNCTION()
	void AnimNotify_CollisionOn();

	UFUNCTION()
	void AnimNotify_CollisionEnd();

	UFUNCTION()
	void AnimNotify_HeavyAttackHoldIdle();

	UFUNCTION()
	void AnimNotify_HeavyAttackHold();

	UFUNCTION()
	void AnimNotify_CheckRollReInput();

	UFUNCTION()
	void AnimNotify_JumpEnd();

	UFUNCTION()
	void AnimNotify_EndHit();

	UFUNCTION()
	void AnimNotify_ApplyExecutionDamage();

	UFUNCTION()
	void AnimNotify_UseItem();

	UFUNCTION()
	void AnimNotify_ConsumeItem();

	UFUNCTION()
	void AnimNotify_ConsumeItemEnd();

	FName GetAttackMontageSectionName(int32 Section);

private:
	bool IsHoldingHeavyAttackKey = false;

private:
	class ABaseCharacter* SourceCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float UpDownAxisValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float LeftRightAxisValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool HasTarget;

public:
	UPROPERTY(BlueprintReadWrite)
	bool IsDeathAnimTriggered = false;

	UPROPERTY(BlueprintReadOnly)
	bool ConsumingItem = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, Meta = (AllowPrivateAccess = true))
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RollMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* BlockHitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* KnockDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, Meta = (AllowPrivateAccess = true))
	UAnimMontage* LightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	UAnimMontage* Player_ExecutionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* SpawnMontage;
};
