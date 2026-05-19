// Fill out your copyright notice in the Description page of Project Settings.


#include "ABEnemyCharacter.h"
#include "ABEnemyAnimInstance.h"
#include "ABAIController.h"
#include "BrainComponent.h"
#include "BaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "ABEnemyStatComponent.h"
#include "EnemyWidget.h"
#include "TargetIndicatorWidget.h"
#include "BonfireSubsystem.h"
#include "ItemDropActor.h"
#include "ABCharacterStatComponent.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AABEnemyCharacter::AABEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WEAPON_COLLIDER"));
	TargetIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TARGETWIDGET"));
	EnemyWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ENEMYWIDGET"));

	WeaponMesh->SetupAttachment(GetMesh(), FName(TEXT("hand_rSocket")));
	WeaponMesh->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	WeaponMesh->SetCollisionProfileName(TEXT("Weapon"));

	TargetIndicatorWidgetComponent->SetupAttachment(GetMesh());
	EnemyWidgetComponent->SetupAttachment(GetMesh());

	WeaponCollider->SetupAttachment(WeaponMesh);
	WeaponCollider->SetRelativeLocation(FVector(-6.0f, 0.0f, 77.0f));
	WeaponCollider->SetBoxExtent(FVector(10.0f, 5.0f, 65.0f));

	TargetIndicatorWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	TargetIndicatorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TargetIndicatorWidgetComponent->SetDrawSize(FVector2D(25.0f, 25.0f));
	TargetIndicatorWidgetComponent->SetVisibility(false);

	EnemyWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 210.0f));
	EnemyWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	EnemyWidgetComponent->SetDrawSize(FVector2D(150.0f, 30.0f));
	EnemyWidgetComponent->SetVisibility(false);

	EnemyStatManager = CreateDefaultSubobject<UABEnemyStatComponent>(TEXT("ENEMYSTATMANAGER"));
	AddInstanceComponent(EnemyStatManager);

	IsBeingTargeted = false;
	IsDead = false;
	IsReadyToExecute = false;
	IsPlayerCharacterBehind = false;
	IsKnockedDown = false;
}

// Called when the game starts or when spawned
void AABEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		if (GI->CurrentSave->DeadEnemies.Contains(UniqueID))
		{
			Destroy();
			return;
		}
	}

	SetDefaultTransform(GetActorTransform());

	GetGameInstance()->GetSubsystem<UBonfireSubsystem>()->RegisterEnemy(this);

	WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//AnimInstance = Cast<UABEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	PlayerCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->RegisterEnemies(this);
	}

	EnemyWidget = Cast<UEnemyWidget>(EnemyWidgetComponent->GetUserWidgetObject());

	if (EnemyWidget)
	{
		EnemyWidget->InitEnemyWidget(this);
	}

	TargetIndicatorWidget = Cast<UTargetIndicatorWidget>(TargetIndicatorWidgetComponent->GetUserWidgetObject());

	if (TargetIndicatorWidget)
	{
		TargetIndicatorWidget->SetOwnerEnemyCharacter(this);
	}
}

// Called every frame
void AABEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastDeltaTime = DeltaTime;

	SetTargetIndicatorWidget();

	IsPlayerCharacterBehind = IsPlayerBehind(PlayerCharacter, ExecutionAngle, ExecutionRange);
}

void AABEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UABEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	WeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AABEnemyCharacter::OnWeaponBeginOverlap);
	OnTakePostureDamage.AddDynamic(this, &AABEnemyCharacter::TakePostureDamage);

	EnemyStatManager->InitEnemyData();

	//FOnMontageEnded ExecutionMontageEndedDelegate;
	//ExecutionMontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
	//	{
	//		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Montage End"));

	//		IsBeingExecuted = false;
	//		IsKnockedDown = false;
	//	});

	MontageEndDelegate.BindUObject(this, &AABEnemyCharacter::OnMontageEnded);
	//AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, AnimInstance->Victim_BackStabMontage);
	AnimInstance->OnMontageEnded.AddDynamic(this, &AABEnemyCharacter::OnMontageEnded);
	//AnimInstance->Montage_SetEndDelegate(ExecutionMontageEndedDelegate, AnimInstance->Victim_ExecutionMontage);



}

float AABEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 적이 죽었는지 확인
	if (IsDead) return 0.0f;

	float DamageCalc = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 데미지를 받으면 적 체력 바와 체간 바 표시
	SetEnemyWidgetVisibility(true);

	// EnemyStatComponent에서 필요한 스텟 불러오기
	float CurrentHP = EnemyStatManager->GetCurrentHP();
	float MaxHP = EnemyStatManager->GetMaxHP();
	float CurrentStamina = EnemyStatManager->GetCurrentPosture();
	float MaxStamina = EnemyStatManager->GetMaxPosture();

	// 적 체간 깎기
	TakePostureDamage(1.0f);

	// 죽었는지 체크
	CurrentHP -= DamageCalc;
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		IsDead = true;

		// 현재 적의 상태에 따라 다른 사망 애님 몽타주 재생
		if (IsBeingKnockedDown || IsKnockedDown)
		{
			AnimInstance->Montage_Play(AnimInstance->DeathMontage_KnockDown);
		}
		else if (!IsBeingExecuted || IsGettingUp)
		{
			AnimInstance->Montage_Play(AnimInstance->DeathMontage_Idle);
		}

		UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
		if (GI && GI->CurrentSave)
		{
			GI->CurrentSave->DeadEnemies.AddUnique(UniqueID);
			GI->MarkDirty();
		}
	}

	// 체력과 체간 업데이트
	SaveHP(CurrentHP);

	return DamageCalc;
}

void AABEnemyCharacter::DropItem()
{
	if (!ItemDropActorClass) return;

	FVector DropLoc = GetActorLocation();
	DropLoc.Z = 10.0f;

	for (FDropData Data : ItemDrop.DropItems)
	{
		if (FMath::RandRange(0.0f, 100.0f) <= Data.SpawnRate)
		{
			AItemDropActor* Drop = GetWorld()->SpawnActor<AItemDropActor>(ItemDropActorClass, DropLoc, FRotator::ZeroRotator);
			if (!Drop) continue;

			Drop->ItemID = Data.ItemID;
			Drop->Count = Data.ItemCount;
			Drop->DropType = EItemDropType::EnemyDrop;
			Drop->DropID = FGuid::NewGuid();

			UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
			if (GI && GI->CurrentSave)
			{
				FDroppedItemData DroppedItemData;
				DroppedItemData.ItemID = Data.ItemID;
				DroppedItemData.Count = Data.ItemCount;
				DroppedItemData.Transform = Drop->GetActorTransform();
				DroppedItemData.DropID = Drop->DropID;

				GI->CurrentSave->DroppedItems.Add(DroppedItemData);

				GI->MarkDirty();
			}
		}
	}
}

//void AABEnemyCharacter::ResetEnemy()
//{
//	GetController<AABAIController>()->BrainComponent->StopLogic(FString::Printf(TEXT("BonfireReset")));
//}

void AABEnemyCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool IsAttackStarted = false;

	// 애님 인스턴스에서 공격 애니메이션이 시작 되었는지 확인
	if (::IsValid(AnimInstance)) IsAttackStarted = AnimInstance->IsAttackStarted;
	if (IsAttackStarted)
	{
		// 무기 콜라이더와 오버랩 된 액터가 플레이어가 맞는지 확인
		if (OtherActor == PlayerCharacter)
		{
			if (PlayerCharacter->IsDead) return;

			// 공격 중복 처리 방지
			if (!PlayerCharacter->IsAttacked)
			{
				PlayerCharacter->IsAttacked = true;

				// 플레이어가 구르는 중인지 확인
				if (!PlayerCharacter->IsRolling)
				{
					// 플레이어가 공격을 받으면 플레이어 런치
					FVector LaunchForce = CalculateLaunchForce(PlayerCharacter);
					PlayerCharacter->LaunchCharacter(LaunchForce, true, true);

					// 플레이어가 가드하고 있는지 확인
					if (PlayerCharacter->IsBlocking)
					{
						// 플레이어가 패링이 가능한 타이밍인지
						if (PlayerCharacter->IsAbleToParry)
						{
							// 패링 성공
							PlayerCharacter->ParrySucceeded = true;

							// 플레이어가 적의 공격을 가드했을 때의 델리게이트
							PlayerCharacter->OnBlockHit.Broadcast(true, 0.0f, this);
						}
						else
						{
							// 플레이어가 다음 공격에 죽을 예정인지 확인
							if (!PlayerCharacter->CheckWouldPlayerDie(InitDamage))
							{
								// 플레이어가 적의 공격을 가드했을 때의 델리게이트
								PlayerCharacter->OnBlockHit.Broadcast(false, PostureDamage, this);
							}

							// 패링에 성공하지 못하면 데미지 적용
							FDamageEvent DamageEvent;
							PlayerCharacter->TakeDamage(InitDamage, DamageEvent, GetController(), this);
						}
					}
					else
					{
						// 플레이어가 다음 공격에 죽을 예정인지 확인
						if (!PlayerCharacter->CheckWouldPlayerDie(InitDamage))
						{
							// 플레이어가 직접 공격을 받았을 때의 델리게이트
							PlayerCharacter->OnDirectHit.Broadcast();
						}

						// 데미지 적용
						FDamageEvent DamageEvent;
						PlayerCharacter->TakeDamage(InitDamage, DamageEvent, GetController(), this);
					}
				}
			}
		}
	}
}

void AABEnemyCharacter::SetIsBeingTargeted(bool bIsBeingTargeted)
{
	IsBeingTargeted = bIsBeingTargeted;

	SetEnemyWidgetVisibility(IsBeingTargeted);
}

FAttackSequenceSectionData AABEnemyCharacter::GetAttackSequenceData(FString CurrentAttackSequence)
{
	FString AttackSequenceNum;
	FString AttackSequenceSectionNum;
	FString Misc;

	FAttackSequenceSectionData AttackSequenceData;

	CurrentAttackSequence.Split(FString::Printf(TEXT("_")), &AttackSequenceNum, &AttackSequenceSectionNum);
	AttackSequenceNum.Split(FString::Printf(TEXT("AttackSequence")), &Misc, &AttackSequenceNum);
	//AttackSequenceNum.Split(FString::Printf(TEXT("0")), &Misc, &AttackSequenceNum);

	if (AttackSequenceDataTable != nullptr)
	{
		if (AttackSequenceNum.IsNumeric() && AttackSequenceSectionNum.IsNumeric())
		{
			FAttackSequenceData* Data = AttackSequenceDataTable->FindRow<FAttackSequenceData>(FName(*AttackSequenceNum), TEXT(""));
			if (Data != nullptr)
			{
				AttackSequenceData = Data->Section[(FCString::Atoi(*AttackSequenceSectionNum) - 1)];
			}
		}
	}

	return AttackSequenceData;
}

void AABEnemyCharacter::SetAttackSequenceData(FAttackSequenceSectionData AttackSequenceData)
{
	InitDamage = AttackSequenceData.Damage;
	PostureDamage = AttackSequenceData.PostureDamage;
}

float AABEnemyCharacter::CalculateFinalDamage(class ABaseCharacter* DamagedPlayerCharacter, float InDamage, bool IsBlocking)
{
	float Damage;
	float ReductionMultiplier = DamagedPlayerCharacter->GuardDamageReductionPercentage / 100.0f;

	if (IsBlocking)
	{
		Damage = InDamage * ReductionMultiplier;
	}
	else
	{
		Damage = InDamage;
	}

	return Damage;
}

FVector AABEnemyCharacter::CalculateLaunchForce(AActor* Victim)
{
	//FVector VictimLoc = Victim->GetActorLocation();
	//FVector AttackerLoc = GetActorLocation();

	//FVector HitDir = (VictimLoc - AttackerLoc).GetSafeNormal();

	//FVector LaunchForce = HitDir * Force;

	FVector Direction = Victim->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	FVector LaunchVelocity = Direction * LaunchStrength;

	return LaunchVelocity;
}

void AABEnemyCharacter::SetEnemyWidgetVisibility(bool bValue)
{
	if (bValue)
	{
		EnemyWidgetComponent->SetVisibility(true);
	}
	else
	{
		if (GetWorldTimerManager().IsTimerActive(EnemyWidgetTimer))
		{
			GetWorldTimerManager().ClearTimer(EnemyWidgetTimer);
		}
		GetWorldTimerManager().SetTimer(EnemyWidgetTimer, [this]()
			{
				EnemyWidgetComponent->SetVisibility(false);
			}, 5.0f, false);
	}
}

void AABEnemyCharacter::SetTargetIndicatorWidget()
{
	FTransform Transform = GetMesh()->GetBoneTransform(TEXT("spine_02"), ERelativeTransformSpace::RTS_Component);
	TargetIndicatorWidgetComponent->SetRelativeLocation(Transform.GetLocation());

	SetTargetWidgetVisibility();
}

bool AABEnemyCharacter::IsPlayerBehind(ABaseCharacter* PlayerChar, float BehindAngleDegree, float Range)
{
	if (!PlayerChar || !::IsValid(PlayerChar)) return false;

	FVector EnemyForwardVec = GetActorForwardVector();
	FVector DirectionToPlayer = (PlayerChar->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector PlayerForwardVec = PlayerChar->GetActorForwardVector().GetSafeNormal();

	float Dot = FVector::DotProduct(EnemyForwardVec, DirectionToPlayer);
	float Threshold = FMath::Cos(FMath::DegreesToRadians(BehindAngleDegree));
	float DistanceToPlayer = GetDistanceTo(PlayerChar);

	float Dot_PlayerFacingEnemy = FVector::DotProduct(PlayerForwardVec, -DirectionToPlayer);
	float ViewAngle = FMath::RadiansToDegrees(FMath::Acos(Dot_PlayerFacingEnemy));

	bool Result = Dot < Threshold && DistanceToPlayer < Range && ViewAngle < PlayerChar->ViewAngle;

	return Result;
}

void AABEnemyCharacter::RegainPosture()
{
	if (!EnemyWidget) return;

	RegainingPosture = true;

	float CurrentPosture = EnemyStatManager->GetCurrentPosture();
	float MaxPosture = EnemyStatManager->GetMaxPosture();

	if (CurrentPosture <= 0.0f)
	{
		CurrentPosture = 0.0f;
		RegainingPosture = false;
		GetWorldTimerManager().ClearTimer(PostureRegainTimerHandle);
	}
	else
	{
		CurrentPosture -= LastDeltaTime * PostureRegainMultiplier;
	}

	EnemyStatManager->SetCurrentPosture(CurrentPosture);
	EnemyWidget->OnPostureBarUpdated.Broadcast(CurrentPosture / MaxPosture);
}

void AABEnemyCharacter::TakePostureDamage(float PD)
{
	// 스탯 컴포넌트가 없으면 리턴
	if (!EnemyStatManager) return;

	// 적 스탯 컴포넌트에서 수치 로드
	float CurrentPosture = EnemyStatManager->GetCurrentPosture();
	float MaxPosture = EnemyStatManager->GetMaxPosture();

	// 적이 앞잡이나 뒤잡 당하고 있거나 체간 무너진 상태인지 체크
	if (!IsBeingKnockedDown && !IsBeingExecuted)
	{
		// 체간 데미지 추가
		CurrentPosture += PD;

		// 체간이 최대 체간 이상으로 올라가면 적이 넘어지는 애니메이션 재생, 그렇지 않으면 체간 수치만 업데이트
		if (CurrentPosture >= MaxPosture)
		{
			// 그로기 몽타주 실행
			AnimInstance->Montage_Play(AnimInstance->KnockedDownMontage);
			IsBeingKnockedDown = true;

			// 무기 콜리젼 박스 콜리젼 off
			WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// 체간 리셋
			CurrentPosture = 0.0f;
			ResetPostureRegain();
		}
		else
		{
			// 체간 회복 타이머가 이미 활성화되어 있으면 초기화하고 새로 타이머 시작
			if (GetWorldTimerManager().IsTimerActive(PostureRegainTimerHandle))
			{
				GetWorldTimerManager().ClearTimer(PostureRegainTimerHandle);
			}
			GetWorldTimerManager().SetTimer(PostureRegainTimerHandle, this, &AABEnemyCharacter::RegainPosture, LastDeltaTime, true, PostureRegainDelaySeconds);
		}
	}

	// 스탯 컴포넌트에 수치 저장
	SavePosture(CurrentPosture);
}

void AABEnemyCharacter::SaveHP(float NewCurrentHP)
{
	if (!EnemyStatManager) return;

	float MaxHP = EnemyStatManager->GetMaxHP();

	EnemyStatManager->SetCurrentHP(NewCurrentHP);

	if (EnemyWidget)
	{
		EnemyWidget->OnHPBarUpdated.Broadcast(NewCurrentHP / MaxHP);
	}
}

void AABEnemyCharacter::SavePosture(float NewCurrentPosture)
{
	float MaxStamina = EnemyStatManager->GetMaxPosture();

	EnemyStatManager->SetCurrentPosture(NewCurrentPosture);

	if (EnemyWidget)
	{
		EnemyWidget->OnPostureBarUpdated.Broadcast(NewCurrentPosture / MaxStamina);
	}
}

void AABEnemyCharacter::SetTargetWidgetVisibility()
{
	if (IsBeingTargeted)
	{
		TargetIndicatorWidgetComponent->SetVisibility(true);
	}
	else
	{
		if (IsBeingExecuted)
		{
			TargetIndicatorWidgetComponent->SetVisibility(false);
		}
		else
		{
			TargetIndicatorWidgetComponent->SetVisibility(IsKnockedDown || IsPlayerCharacterBehind);
		}
	}
}

void AABEnemyCharacter::ResetPostureRegain()
{
	GetWorldTimerManager().ClearTimer(PostureRegainTimerHandle);
	EnemyStatManager->SetCurrentPosture(0.0f);
}

void AABEnemyCharacter::Respawn()
{
	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->DeadEnemies.Remove(UniqueID);

		GI->MarkDirty();
	}

	SetActorLocationAndRotation(DefaultTransform.GetLocation(), DefaultTransform.GetRotation());
	SetActorEnableCollision(true);

	IsDead = false;

	if (UABEnemyAnimInstance* Anim = Cast<UABEnemyAnimInstance>(AnimInstance))
	{
		Anim->ResetAnimInstance();
	}
	GetMesh()->InitAnim(true);

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();

		if (AICon->GetBlackboardComponent())
		{
			AICon->GetBlackboardComponent()->ClearValue("TargetActor");
			AICon->GetBlackboardComponent()->ClearValue("State");
		}

		AICon->BrainComponent->RestartLogic();
	}

	if (EnemyStatManager)
	{
		EnemyStatManager->InitEnemyData();
	}

	if (EnemyWidget)
	{
		EnemyWidget->OnHPBarUpdated.Broadcast(1.0f);
		EnemyWidget->OnPostureBarUpdated.Broadcast(0.0f);
	}
}

void AABEnemyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AnimInstance->Victim_BackStabMontage || Montage == AnimInstance->Victim_ExecutionMontage)
	{
		IsBeingExecuted = false;
		IsBeingKnockedDown = false;
		IsKnockedDown = false;
		IsGettingUp = false;
		AnimInstance->IsAbleToJumpToNextGetUpSection = false;
	}

	if (Montage == AnimInstance->KnockedDownMontage)
	{
		IsBeingKnockedDown = false;
		IsKnockedDown = false;
		IsGettingUp = false;
	}

	if (Montage == AnimInstance->DeathMontage_Idle || Montage == AnimInstance->DeathMontage_KnockDown)
	{
		DropItem();

		if (PlayerCharacter)
		{
			PlayerCharacter->CharacterStatManager->AddSouls(ItemDrop.DropSouls);
		}
	}
}
