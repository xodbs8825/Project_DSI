// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "ABEnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakePostureDamage, float, PD);

USTRUCT(BlueprintType)
struct FDropData
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount = 0;
};

USTRUCT(BlueprintType)
struct FItemDrop
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DropSouls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDropData> DropItems;
};

USTRUCT(BlueprintType)
struct FAttackSequenceSectionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SectionNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PostureDamage;
};

USTRUCT(BlueprintType)
struct FAttackSequenceData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackSequenceSectionData> Section;
};

UCLASS()
class PROJECT_DSI_API AABEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UBoxComponent* WeaponCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* TargetIndicatorWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* EnemyWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	class UABEnemyStatComponent* EnemyStatManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakePostureDamage(float PD);

	void SaveHP(float NewCurrentHP);
	void SavePosture(float NewCurrentPosture);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//void ResetEnemy();

	void SetIsDead(bool bIsDead)
	{
		IsDead = bIsDead;
		//if (IsDead)
		//{
		//	GetController<AABAIController>()->BrainComponent->StopLogic(FString::Printf(TEXT("PawnDead")));
		//}
	}
	void SetIsBeingTargeted(bool bIsBeingTargeted);
	void SetIsAttacked(bool bIsAttacked) { IsAttacked = bIsAttacked; }
	void SetDefaultTransform(FTransform Transform) { DefaultTransform = Transform; }
	void SetEnemyAnimInstance(class UABEnemyAnimInstance* Anim) { AnimInstance = Anim; }

	bool GetIsDead() { return IsDead; }
	FTransform GetDefaultTransform() { return DefaultTransform; }
	class UABEnemyAnimInstance* GetEnemyAnimInstance() { return AnimInstance; }

	bool DoesEnemyAnimInstanceExist()
	{ 
		if (GetEnemyAnimInstance() != nullptr) return true;
		else return false;
	}

	// BP 에서 실행
	UFUNCTION(BlueprintCallable)
	FAttackSequenceSectionData GetAttackSequenceData(FString CurrentAttackSequence);

	UFUNCTION(BlueprintCallable)
	void SetAttackSequenceData(FAttackSequenceSectionData AttackSequenceData);

	void ResetPostureRegain();

	void Respawn();

private:
	float CalculateFinalDamage(class ABaseCharacter* DamagedPlayerCharacter, float InDamage, bool IsBlocking);
	FVector CalculateLaunchForce(AActor* Victim);

	void SetEnemyWidgetVisibility(bool bValue);

	void SetTargetIndicatorWidget();

	bool IsPlayerBehind(ABaseCharacter* PlayerCharacter, float BehindAngleDegree, float Range);

	void RegainPosture();

	void SetTargetWidgetVisibility();

public:
	UPROPERTY(EditAnywhere)
	FName UniqueID;

	bool PlayerBonfireReset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsAttacked = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float InitDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float FinalDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float PostureDamage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UDataTable* AttackSequenceDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = true))
	class UABEnemyAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsBeingKnockedDown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsKnockedDown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsPlayerCharacterBehind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsBeingExecuted;

	UPROPERTY(EditAnywhere)
	float LaunchStrength = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "ItemDrop")
	TSubclassOf<class AItemDropActor> ItemDropActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Drop")
	FItemDrop ItemDrop;

	bool IsGettingUp = false;

	class ABaseCharacter* PlayerCharacter;

	FOnTakePostureDamage OnTakePostureDamage;

	FOnMontageEnded MontageEndDelegate;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	FTimerHandle EnemyWidgetTimer;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatus", meta = (AllowPrivateAccess = true))
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsBeingTargeted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool IsReadyToExecute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float ExecutionAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float ExecutionRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowprivateAccess = true))
	FTransform DefaultTransform;

	float LastDeltaTime = 0.0f;

	bool RegainingPosture = false;
	int32 PostureRegainDelaySeconds = 3.0f;
	int32 PostureRegainMultiplier = 1.0f;
	FTimerHandle PostureRegainTimerHandle;

	class UEnemyWidget* EnemyWidget;
	class UTargetIndicatorWidget* TargetIndicatorWidget;
};
