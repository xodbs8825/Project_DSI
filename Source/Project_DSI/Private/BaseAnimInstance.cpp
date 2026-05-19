// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"

UBaseAnimInstance::UBaseAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	UpDownAxisValue = 0.0f;
	LeftRightAxisValue = 0.0f;

	IFrame = false;
	JumpRecovery = false;

	IsSprinting = false;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	CurrentPawnSpeed = Pawn->GetVelocity().Size();

	if (::IsValid(SourceCharacter))
	{
		IsInAir = SourceCharacter->GetMovementComponent()->IsFalling();
		HasTarget = SourceCharacter->HasTarget;
		IsDead = SourceCharacter->IsDead;
	}
	//if (IsSprinting)
	//{
	//	if (!HasTarget)
	//	{
	//		UpDownAxisValue = 1.0f;
	//	}
	//	else
	//	{
	//		if (UpDownAxisValue > 0.0f)
	//		{
	//			UpDownAxisValue += DeltaSeconds;
	//		}
	//		else if (UpDownAxisValue < 0.0f)
	//		{
	//			UpDownAxisValue -= DeltaSeconds;
	//		}
	//	}
	//}
	//else
	//{
	//	if (!HasTarget)
	//	{
	//		UpDownAxisValue = .66f;
	//	}
	//}
}

void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (ABaseCharacter* Character = Cast<ABaseCharacter>(TryGetPawnOwner()))
	{
		SourceCharacter = Character;

		if (LightAttackMontage != NULL)
		{
			Character->SetMaxLightAttackMontageSection(LightAttackMontage->GetNumSections());
		}

		if (HeavyAttackMontage != NULL)
		{
			Character->SetMaxHeavyAttackMontageSection(HeavyAttackMontage->GetNumSections());
		}
	}
}

void UBaseAnimInstance::PlayLightAttackMontage()
{
	if (IsDead) return;

	Montage_Play(LightAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f);
}

void UBaseAnimInstance::PlayHeavyAttackMontage()
{
	if (IsDead) return;

	Montage_Play(HeavyAttackMontage, 1.0f);
}

void UBaseAnimInstance::SetHeavyAttackPlayRate(float PlayRate)
{
	if (!Montage_IsPlaying(HeavyAttackMontage)) return;

	Montage_SetPlayRate(HeavyAttackMontage, PlayRate);
}

void UBaseAnimInstance::JumpToLightAttackMontageSection(int32 NewSection)
{
	if (IsDead) return;
	if (!Montage_IsPlaying(LightAttackMontage)) return;

	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), LightAttackMontage);
}

void UBaseAnimInstance::JumpToHeavyAttackMontageSection(int32 NewSection)
{
	if (IsDead) return;
	if (!Montage_IsPlaying(HeavyAttackMontage)) return;

	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), HeavyAttackMontage);
}

void UBaseAnimInstance::SetAxisValue(FVector2D MovementVector)
{
	UpDownAxisValue = MovementVector.Y;
	LeftRightAxisValue = MovementVector.X;
}

void UBaseAnimInstance::Reset()
{
	//CurrentPawnSpeed = 0.0f;
	//UpDownAxisValue = 0.0f;
	//LeftRightAxisValue = 0.0f;

	IsInAir = false;
	IsDead = false;
	IFrame = false;
	JumpRecovery = false;
}

void UBaseAnimInstance::SetSprintSpeed(float Speed)
{
	SprintSpeed = Speed;

	if (SprintSpeed == 1.2f) IsSprinting = true;
	else IsSprinting = false;
}

#pragma region AnimNotify
//void UBaseAnimInstance::AnimNotify_AttackHitCheck()
//{
//	SourceCharacter->Weapon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//}

void UBaseAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UBaseAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UBaseAnimInstance::AnimNotify_EndAttack()
{
	OnEndAttack.Broadcast();
}

void UBaseAnimInstance::AnimNotify_CollisionOn()
{
	OnCollisionOn.Broadcast();
}

void UBaseAnimInstance::AnimNotify_CollisionEnd()
{
	OnCollisionEnd.Broadcast();
}

void UBaseAnimInstance::AnimNotify_HeavyAttackHoldIdle()
{
	if (IsHoldingHeavyAttackKey)
	{
		SetHeavyAttackPlayRate(0.1f);
	}
}

void UBaseAnimInstance::AnimNotify_HeavyAttackHold()
{
	OnHeavyAttackHold.Broadcast();

	if (IsHoldingHeavyAttackKey)
	{
		SetHeavyAttackPlayRate(-0.1f);
	}
}

void UBaseAnimInstance::AnimNotify_CheckRollReInput()
{
	SourceCharacter->CanReRoll = true;
}

void UBaseAnimInstance::AnimNotify_JumpEnd()
{
	JumpRecovery = false;
}

void UBaseAnimInstance::AnimNotify_EndHit()
{
	SourceCharacter->IsAbleToRoll = true;
	SourceCharacter->IsAbleToMove = true;
}

void UBaseAnimInstance::AnimNotify_ApplyExecutionDamage()
{
	OnExecutionDamage.Execute();
}

void UBaseAnimInstance::AnimNotify_UseItem()
{
	OnItemUsed.Execute();
}

void UBaseAnimInstance::AnimNotify_ConsumeItem()
{
	SourceCharacter->IsAbleToMove = true;
	ConsumingItem = true;
}

void UBaseAnimInstance::AnimNotify_ConsumeItemEnd()
{
	ConsumingItem = false;
}

//void UBaseAnimInstance::AnimNotify_CollEndPoint()
//{
//	SourceCharacter->Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	OnCollEnd.Broadcast();
//}
#pragma endregion

FName UBaseAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	if (!FMath::IsWithinInclusive<int32>(Section, 1, 4)) return NAME_None;

	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}