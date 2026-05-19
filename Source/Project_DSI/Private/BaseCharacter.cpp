#include "BaseCharacter.h"
#include "ABEnemyCharacter.h"
#include "BaseAnimInstance.h"
#include "ABEnemyAnimInstance.h"
#include "BaseItem.h"
#include "NPCBase.h"
#include "ABAIController.h"
#include "ABPlayerState.h"
#include "Bonfire.h"
#include "PlayerHUD.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BrainComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InventoryManager.h"
#include "ABCharacterStatComponent.h"
#include "NiagaraComponent.h"
#include "InteractionComponent.h"
#include "ABPlayerController.h"
#include "WeaponBaseActor.h"
#include "SoulsDropActor.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "SafeLocationFinder.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	CharacterStatManager = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CharacterStatManager"));
	AddInstanceComponent(CharacterStatManager);

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("InventoryManager"));
	AddInstanceComponent(InventoryManager);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	SetControlMode(0);

	GetCharacterMovement()->JumpZVelocity = 600;

	IsLightAttacking = false;

	MaxLightAttackCombo = 4;
	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("TestCharacter"));

	CurrentItem = nullptr;

	LockOnPressed = false;
	HasTarget = false;
	TargetInRange = false;

	Target = nullptr;
	MaxTargetSearchDistance = 2000;
	CameraRotationSpeed = 25;

	IsDead = false;
	IsBeingTargeted = false;

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GuardDamageReductionPercentage = FMath::Clamp(50.0f, 0.0f, 100.0f);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		const UDSI_SaveGame* Save = GI->CurrentSave;

		FTransform SpawnTransform;

		if (Save->bHasValidPlayerTransform)
		{
			SpawnTransform = Save->PlayerTransform;

			UCapsuleComponent* Capsule = GetCapsuleComponent();
			FVector CapsuleExtent = FVector(Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight());

			FVector SafeLocation;
			USafeLocationFinder::FindSafeLocation(this, SpawnTransform.GetLocation(), CapsuleExtent, SafeLocation);

			SpawnTransform.SetLocation(SafeLocation);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Spawn from Last Position"));
			}
		}
		else
		{
			AActor* Start = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
			if (Start)
			{
				SpawnTransform = Start->GetActorTransform();

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Spawn from Start"));
				}
			}
		}

		SetActorTransform(SpawnTransform);
		GetController()->SetControlRotation(SpawnTransform.Rotator());

		GetWorldTimerManager().SetTimer(PlayerTransformUpdateTimer, this, &ABaseCharacter::UpdatePlayerTransform, GI->PlayerTransformUpdateInterval, true);

		if (GI->CurrentSave->EquipmentData.Weapon.ItemID != TEXT(""))
		{
			const FWeaponData* WeaponData = InventoryManager->GetWeaponData(GI->CurrentSave->EquipmentData.Weapon.ItemID);
			if (WeaponData)
			{
				SetWeapon(WeaponData);
				CharacterStatManager->SetWeaponData(WeaponData);
			}
		}
	}

	PlayerController = Cast<AABPlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->SetInputMappingContext(IMC_Default);

		PlayerController->OnUIOpened.AddDynamic(this, &ABaseCharacter::UIOpened);
		PlayerController->OnUIClosed.AddDynamic(this, &ABaseCharacter::UIClosed);

		PlayerController->CreateDeathWidget();
	}

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			if (GI && GI->CurrentSave)
			{
				if (GI->CurrentSave->bHasHotkeyItems)
				{
					PlayerHUDWidget->RefreshHotkey();
				}
			}
		}
	}

	SpawnPlayer();
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Anim = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == Anim) return;

	TargetCharacters.Empty();

	Anim->OnMontageStarted.AddDynamic(this, &ABaseCharacter::OnMontageStarted);
	Anim->OnMontageEnded.AddDynamic(this, &ABaseCharacter::OnMontageEnded);

	Anim->OnNextAttackCheck.AddLambda([this]() -> void
		{
			CanNextLightAttackCombo = false;
			CanReRoll = false;

			if (IsLightAttackComboInputOn)
			{
				IsLightAttackComboInputOn = false;
				CurrentLightAttackCombo = FMath::Clamp(CurrentLightAttackCombo + 1, 1, MaxLightAttackCombo);

				if (!HasTarget) ChangeAnimMontageDirection();
				Anim->JumpToLightAttackMontageSection(CurrentLightAttackCombo);
			}
			else if (IsReRollInputOn)
			{
				IsReRollInputOn = false;

				if (!HasTarget) ChangeAnimMontageDirection();
				PlayRollMontage();
			}
			else
			{
				IsAbleToMove = true;
			}
		});

	Anim->OnAttackHitCheck.AddLambda([this]() -> void
		{
			CanNextLightAttackCombo = true;
		});

	// 실질적으로 플레이어가 공격을 하기 시작하는 시점의 노티파이에서 Broadcast
	Anim->OnCollisionOn.AddLambda([this]() -> void
		{
			// 무기 액터가 있는지 확인
			if (WeaponActor)
			{
				// 무기 액터에 달려있는 콜리전 박스의 콜리전 On
				WeaponActor->WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				// 공격 시 사용하는 스테미나 차감 (현재 테스트 용으로 5 차감)
				SpendStamina(5);
			}
		});

	Anim->OnCollisionEnd.AddLambda([this]() -> void
		{
			if (WeaponActor)
			{
				WeaponActor->WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			for (AABEnemyCharacter* Character : AttackedEnemies)
			{
				Character->SetIsAttacked(false);
			}

			AttackedEnemies.Empty();
		});


	OnBlockHit.AddDynamic(this, &ABaseCharacter::BlockHit);

	OnDirectHit.AddLambda([this]()
		{
			// 현재 열려있는 위젯 (인벤토리, 장비창 등)을 모두 닫음
			PlayerController->CloseAllWidget();

			// 플레이어가 그로기 상태에 들어가 있는지 확인
			if (!KnockDown && !IsExecuting)
			{
				// 그로기에 들어가 있지 않으면 히트 몽타주 실행
				DirectHit = true;
				Anim->Montage_Play(Anim->HitMontage, 1.5f);
			}

			// 스태미나 회복 타이머 리셋
			SetStaminaRegainTimer();
		});
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsPlayerControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}
}

void ABaseCharacter::SetControlMode(int32 ControlMode)
{
	if (ControlMode == 0)
	{
		SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

		SpringArm->TargetArmLength = 450.0f;

		SpringArm->bUsePawnControlRotation = true;

		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		SpringArm->bDoCollisionTest = true;

		Camera->bUsePawnControlRotation = false;

		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;

		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}
}

void ABaseCharacter::UIOpened(AABPlayerController* PC)
{
	PC->SetInputMappingContext(IMC_UI);
}

void ABaseCharacter::UIClosed(AABPlayerController* PC)
{
	PC->SetInputMappingContext(IMC_Default);
}

void ABaseCharacter::BlockHit(bool bParrySucceeded, int32 StaminaDamage, AABEnemyCharacter* Enemy)
{
	// 현재 열려있는 위젯 (인벤토리, 장비창 등)을 모두 닫음
	PlayerController->CloseAllWidget();

	// 패리 성공 여부 확인
	ParrySucceeded = bParrySucceeded;
	if (ParrySucceeded)
	{
		// 적의 공격을 성공적으로 패리 했으면 적의 체간을 깎음
		Enemy->OnTakePostureDamage.Broadcast(PostureDamage);
		IsAttacked = false;
	}
	else
	{
		// 적의 공격을 패리하지 못했으면 스태미나가 깎임 (테스트 용으로 10 차감)
		SpendStamina(10.0f);
	}

	// 가드 나이아가라 이펙트 설정
	if (WeaponActor)
	{
		WeaponActor->BlockHitFX->SetBoolParameter(TEXT("ParrySuccess"), ParrySucceeded);
		WeaponActor->BlockHitFX->Activate(true);
	}
	bBlockHit = true;

	IsAbleToRoll = false;

	// 피격 몽타주가 실행 중이면 종료
	if (Anim->Montage_IsPlaying(Anim->HitMontage))
	{
		Anim->Montage_Stop(0.0f, Anim->HitMontage);
	}

	// 스태미나가 0 아래로 떨어지면 플레이어가 그로기 상태에 들어감
	if (CharacterStatManager->CurrentStamina <= 0.0f)
	{
		// 그로기에 들어가 있는 상태면 데미지만 입음
		if (!KnockDown)
		{
			KnockDown = true;

			Anim->Montage_Play(Anim->KnockDownMontage);

			bBlockHit = false;
			GetWorldTimerManager().ClearTimer(StaminaRegainTimerHandle);
		}
	}
	else
	{
		// 가드 충격 몽타주 실행
		Anim->Montage_Play(Anim->BlockHitMontage, 1.5f);

		// 스태미나 회복 타이머 세팅
		SetStaminaRegainTimer();
	}

	// 패리 타이밍을 위한 bool 변수 리셋
	IsAbleToParry = false;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastDeltaTime = DeltaTime;

	if (!GetWorldTimerManager().IsTimerActive(StaminaRegainTimerHandle) && CharacterStatManager->CurrentStamina > 0.0f)
	{
		GetWorldTimerManager().SetTimer(StaminaRegainTimerHandle, this, &ABaseCharacter::RegainStamina, LastDeltaTime, true, StaminaRegainDelaySeconds);
	}
}

#pragma region Input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//UInputTriggerHold* HoldTrigger = NewObject<UInputTriggerHold>();
		//SprintAction->Triggers.Add(HoldTrigger);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABaseCharacter::MoveEnd);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ABaseCharacter::MoveEnd);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABaseCharacter::SprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABaseCharacter::SprintTriggered);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABaseCharacter::SprintEnd);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ABaseCharacter::SprintEnd);

		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ABaseCharacter::LightAttack);
		//EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ABaseCharacter::HeavyAttack);
		//EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ABaseCharacter::HeavyAttackEnd);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ABaseCharacter::Roll);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ABaseCharacter::Interact);
		EnhancedInputComponent->BindAction(LockOn_ResetCameraAction, ETriggerEvent::Started, this, &ABaseCharacter::LockOn_ResetCamera);
		EnhancedInputComponent->BindAction(ChangeTargetAction, ETriggerEvent::Triggered, this, &ABaseCharacter::ChangeTarget);

		//EnhancedInputComponent->BindAction(HotkeyIndexUpAction, ETriggerEvent::Started, this, &ABaseCharacter::HotkeyIndexUp);
		//EnhancedInputComponent->BindAction(HotkeyIndexDownAction, ETriggerEvent::Started, this, &ABaseCharacter::HotkeyIndexDown);
		//EnhancedInputComponent->BindAction(UseHotkeyAction, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey);

		EnhancedInputComponent->BindAction(UseHotkey_1, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey, 1);
		EnhancedInputComponent->BindAction(UseHotkey_2, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey, 2);
		EnhancedInputComponent->BindAction(UseHotkey_3, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey, 3);
		EnhancedInputComponent->BindAction(UseHotkey_4, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey, 4);
		EnhancedInputComponent->BindAction(UseHotkey_5, ETriggerEvent::Started, this, &ABaseCharacter::UseHotkey, 5);

		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ABaseCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(EquipmentAction, ETriggerEvent::Started, this, &ABaseCharacter::ToggleEquipment);
		EnhancedInputComponent->BindAction(CloseUIWidget, ETriggerEvent::Started, this, &ABaseCharacter::CloseWidget);

		EnhancedInputComponent->BindAction(MiscAction, ETriggerEvent::Started, this, &ABaseCharacter::Misc);

	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	if (IsDead) return;
	if (IsSpawning) return;

	Axis2D = Value.Get<FVector2D>();
	IsMoveKeyPressed = true;

	if (Anim->JumpRecovery) return;
	if (Anim->IsInAir) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (IsAttacked) return;
	if (Anim->IsAnyMontagePlaying())
	{
		if (!IsAbleToMove)
		{
			return;
		}
	}

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ForwardVec = ForwardDirection;
		RightVec = RightDirection;

		UpAxis = Axis2D.Y;
		RightAxis = Axis2D.X;

		AddMovementInput(ForwardDirection, Axis2D.Y);
		AddMovementInput(RightDirection, Axis2D.X);

		Anim->SetAxisValue(Axis2D);

		if (HasTarget)
		{
			if (QuickTurn || IsSprinting || IsRolling)
			{
				GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
				GetCharacterMovement()->bOrientRotationToMovement = true;
				bUseControllerRotationYaw = false;
			}
			else
			{
				SetActorRotation(FRotator::ZeroRotator);
			}
		}
		else if (!Anim->IsRolling || !IsBlocking)
		{
			GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bUseControllerRotationYaw = false;
		}
	}
}

void ABaseCharacter::MoveEnd()
{
	IsMoveKeyPressed = false;
}

void ABaseCharacter::SprintStarted(const FInputActionInstance& Instance)
{
	if (IsDead) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (IsBlocking) return;
	if (IsExhausted) return;
	if (IsSpawning) return;

	IsHoldingSprint = true;
	HoldTime = 0.0f;
}

void ABaseCharacter::SprintTriggered(const FInputActionInstance& Instance)
{
	if (IsDead) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (IsBlocking) return;
	if (!IsHoldingSprint || IsExhausted) return;
	if (IsSpawning) return;
	if (!IsMoveKeyPressed)
	{
		HoldTime = 0.0f;

		return;
	}

	HoldTime += GetWorld()->GetDeltaSeconds();

	if (HoldTime >= 1.0f)
	{
		IsSprinting = true;
		SpendStamina(LastDeltaTime * 10.0f);
	}
}

void ABaseCharacter::SprintEnd()
{
	if (IsDead) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (IsBlocking) return;
	if (IsSpawning) return;

	IsHoldingSprint = false;
	HoldTime = 0.0f;
	IsSprinting = false;
}

void ABaseCharacter::Jump()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) return;
	if (IsLightAttacking) return;
	if (IsDead) return;
	if (bBlockHit) return;
	if (DirectHit) return;

	Super::Jump();

	//Anim->JumpRecovery = true;
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	if (HasTarget) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		GetWorldTimerManager().ClearTimer(ResetCameraTimer);
	}
}

void ABaseCharacter::LightAttack()
{
	if (IsExecuting) return;
	if (Anim->IsRolling) return;
	if (Anim->JumpRecovery) return;
	if (Anim->IsInAir) return;
	if (IsDead) return;
	if (IsHeavyAttacking) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (!IsAbleToAttack) return;
	if (IsAttacked) return;
	if (!WeaponActor) return;
	if (IsSpawning) return;

	if (IsLightAttacking)
	{
		if (CanNextLightAttackCombo)
		{
			IsLightAttackComboInputOn = true;
			//CanNextLightAttackCombo = false;
		}

		return;
	}

	if (Anim->IsAnyMontagePlaying()) return;

	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector End = Start + Forward * ExecutionRange;

	float Radius = 75.0f;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;

	bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, Sphere, Params);

	if (bHit)
	{
		for (auto& Hit : Hits)
		{
			AActor* TargetActor = Hit.GetActor();
			if (TargetActor)
			{
				if (AABEnemyCharacter* TargetChar = Cast<AABEnemyCharacter>(TargetActor))
				{
					if (TargetChar->IsBeingExecuted) break;

					EnemyBeingExecuted = TargetChar;

					if (TargetChar->IsPlayerCharacterBehind)
					{
						TargetChar->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 90.0f);
						TargetChar->SetActorRotation(GetActorRotation());

						// 플레이어가 적을 찌르는 순간에 애님 노티파이에서 Broadcast
						Anim->OnExecutionDamage.BindLambda([this]() -> void
							{
								// 뒤 잡기 데미지 적용 (테스트 용으로 현재 플레이어의 데미지의 1.5배 데미지 적용)
								UGameplayStatics::ApplyDamage(EnemyBeingExecuted, CharacterStatManager->GetOverallDamage() * 1.5f, GetController(), this, UDamageType::StaticClass());
							});

						Anim->Montage_Play(Anim->Player_ExecutionMontage);
						TargetChar->AnimInstance->Montage_Play(TargetChar->AnimInstance->Victim_BackStabMontage);

						IsExecuting = true;
						TargetChar->IsBeingExecuted = true;
						TargetChar->ResetPostureRegain();

						return;
					}
					else if (TargetChar->IsKnockedDown)
					{
						TargetChar->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 90.0f);
						TargetChar->SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + 180.0f, GetActorRotation().Roll));

						// 플레이어가 적을 찌르는 순간에 애님 노티파이에서 Broadcast
						Anim->OnExecutionDamage.BindLambda([this]() -> void
							{
								// 앞 잡기 데미지 적용 (테스트 용으로 현재 플레이어의 데미지의 2배 데미지 적용)
								UGameplayStatics::ApplyDamage(EnemyBeingExecuted, CharacterStatManager->GetOverallDamage() * 2.0f, GetController(), this, UDamageType::StaticClass());
							});

						Anim->Montage_Play(Anim->Player_ExecutionMontage);
						TargetChar->AnimInstance->Montage_Play(TargetChar->AnimInstance->Victim_ExecutionMontage);

						IsExecuting = true;
						TargetChar->IsKnockedDown = false;
						TargetChar->IsBeingExecuted = true;

						return;
					}

					break;
				}
			}
		}
	}

	IsLightAttacking = true;
	CurrentLightAttackCombo = 1;

	Anim->Montage_Play(Anim->LightAttackMontage);
	Anim->JumpToLightAttackMontageSection(1);

	if (!HasTarget) ChangeAnimMontageDirection();
}

void ABaseCharacter::SetWeapon(const FWeaponData* WeaponData)
{
	if (!WeaponData) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	FVector Loc = FVector::Zero();
	FRotator Rot = FRotator(0.0f, 90.0f, 0.0f);

	TSubclassOf<AWeaponBaseActor> WeaponClass = WeaponData->WeaponActor;
	WeaponActor = GetWorld()->SpawnActor<AWeaponBaseActor>(WeaponClass, Loc, Rot, Params);
	if (!WeaponActor) return;

	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_rSocket"));
	WeaponActor->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponActor->WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponActor->WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnWeaponBeginOverlap);
	WeaponActor->BlockHitFX->SetActive(false);
}

void ABaseCharacter::ClearWeapon()
{
	if (!WeaponActor) return;
	WeaponActor->Destroy();
}

void ABaseCharacter::SpendStamina(float Stamina)
{
	// 스탯 컴포넌트에 스태미나 차감
	CharacterStatManager->AddStamina(-Stamina);

	// 스태미나 수치 로드
	float CurrentStamina = CharacterStatManager->CurrentStamina;
	if (CurrentStamina <= 0.0f)
	{
		IsExhausted = true;
		IsHoldingSprint = false;
		HoldTime = false;
		IsSprinting = false;
	}

	// 스태미나 회복 타이머 세팅
	SetStaminaRegainTimer();
}

bool ABaseCharacter::CheckWouldPlayerDie(float ExpectedDamage)
{
	float CurrentHP = CharacterStatManager->CurrentHP;

	return CurrentHP <= ExpectedDamage;
}

void ABaseCharacter::OnDeath()
{
	if (Anim)
	{
		Anim->Montage_Play(Anim->DeathMontage);
		//Anim->IsDeathAnimTriggered = true;
	}

	if (PlayerController)
	{
		PlayerController->TriggerDeathWidget();
	}

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->OnHideHUD.Broadcast();
	}

	IsDead = true;
	//GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ABaseCharacter::Respawn, RespawnDelay, false);

	HasTarget = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	if (!TargetCharacters.IsEmpty())
	{
		for (auto const& Characters : TargetCharacters)
		{
			Characters->SetIsBeingTargeted(false);
		}
	}

	TargetCharacters.Empty();
	Target = nullptr;

	GetWorldTimerManager().ClearTimer(LockOnTimer);
	GetWorldTimerManager().ClearTimer(ResetCameraTimer);
}

void ABaseCharacter::Respawn()
{
	DropSouls();

	FTransform SpawnTransform;

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		FName BonfireID = GI->CurrentSave->LastBonfireID;
		ABonfire* LastBonfire = FindBonfireByID(BonfireID);
		if (LastBonfire)
		{
			SpawnTransform = LastBonfire->GetActorTransform();
		}
		else
		{
			AActor* Start = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
			if (Start)
			{
				SpawnTransform = Start->GetActorTransform();
			}
		}

		GetController()->SetControlRotation(SpawnTransform.Rotator());

		SetActorTransform(SpawnTransform);

		if (Anim)
		{
			Anim->IsDeathAnimTriggered = false;
			Anim->Montage_Play(Anim->SpawnMontage);
		}

		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->OnShowHUD.Broadcast();
		}

		ResetEnemies();
		CharacterStatManager->RecoverFull();

		GI->CurrentSave->PlayerTransform = SpawnTransform;
		GI->MarkDirty();
		GI->SaveGameData();

		IsDead = false;
	}
}

void ABaseCharacter::SpawnPlayer()
{
	if (!Anim) return;
	Anim->Montage_Play(Anim->SpawnMontage);
	IsSpawning = true;
}

void ABaseCharacter::DropSouls()
{
	if (!SoulsDropClass) return;

	FVector Loc = GetActorLocation();

	Loc.Z -= 88.0f;

	ASoulsDropActor* Souls = GetWorld()->SpawnActor<ASoulsDropActor>(SoulsDropClass, Loc, FRotator::ZeroRotator);
	if (!Souls) return;

	Souls->SoulsAmount = CharacterStatManager->GetSouls();
	CharacterStatManager->DropAllSouls(Souls);

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->Souls = 0;
		GI->CurrentSave->DroppedSoulsAmount = Souls->SoulsAmount;
		GI->CurrentSave->DroppedSoulsTransform = Souls->GetActorTransform();

		GI->MarkDirty();
	}
}

ABonfire* ABaseCharacter::FindBonfireByID(FName BonfireID)
{
	for (TActorIterator<ABonfire> It(GetWorld()); It; ++It)
	{
		ABonfire* Bonfire = *It;
		if (Bonfire && Bonfire->UniqueID == BonfireID)
		{
			return Bonfire;
		}
	}

	return nullptr;
}

void ABaseCharacter::RegisterEnemies(AABEnemyCharacter* Enemy)
{
	Enemies.AddUnique(Enemy);
}

void ABaseCharacter::ResetEnemies()
{
	for (auto Enemy : Enemies)
	{
		if (Enemy.IsValid())
		{
			Enemy->Respawn();
		}
	}
}


void ABaseCharacter::HeavyAttack(const FInputActionInstance& Instance)
{
	if (Anim->IsRolling) return;
	if (Anim->JumpRecovery) return;
	if (Anim->IsInAir) return;
	if (IsDead) return;
	if (IsLightAttacking) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (!IsAbleToAttack) return;
	if (IsAttacked) return;

	HeavyAttackPressed = true;

	if (IsHeavyAttacking)
	{
		if (!FMath::IsWithinInclusive<int32>(CurrentHeavyAttackCombo, 1, MaxHeavyAttackCombo)) return;

		if (CanNextHeavyAttackCombo)
		{
			IsHeavyAttackComboInputOn = true;
		}
	}
	else
	{
		if (CurrentHeavyAttackCombo != 0) return;

		IsHeavyAttacking = true;
		HeavyAttackStartComboState();

		Anim->PlayHeavyAttackMontage();
		Anim->JumpToHeavyAttackMontageSection(CurrentHeavyAttackCombo);

		Anim->OnHeavyAttackHold.AddLambda([this]() -> void
			{
				Anim->SetIsHoldingHeavyAttackKey(HeavyAttackPressed);
			});
	}
}

void ABaseCharacter::HeavyAttackEnd()
{
	if (!HeavyAttackPressed) return;

	HeavyAttackPressed = false;

	Anim->SetIsHoldingHeavyAttackKey(HeavyAttackPressed);
	Anim->SetHeavyAttackPlayRate(1.0f);
}

void ABaseCharacter::Roll()
{
	//if (IsLightAttacking) return;
	if (Anim->JumpRecovery) return;
	if (Anim->IsInAir) return;
	if (IsDead) return;
	if (bBlockHit) return;
	if (DirectHit) return;
	if (IsSpawning) return;

	//if (IsRolling)
	//{
	//	if (!IsInIFrame)
	//	{
	//		IsReRollInputOn = true;
	//	}
	//}
	if (IsRolling)
	{
		if (CanReRoll)
		{
			IsReRollInputOn = true;
			PlayRollMontage();
		}

		return;
	}

	if (IsLightAttacking)
	{
		if (CanNextLightAttackCombo)
		{
			IsReRollInputOn = true;
		}

		return;
	}

	if (bBlockHit)
	{
		if (IsAbleToRoll)
		{
			PlayRollMontage();
		}

		return;
	}

	if (Anim->IsAnyMontagePlaying()) return;

	//if ((Anim->IsRolling && Anim->AbleToCheckReInput) || !Anim->IsRolling)
	//{
	//	ChangeAnimMontageDirection();
	//	Anim->PlayRollMontage();
	//}
	PlayRollMontage();
}

void ABaseCharacter::Interact()
{
	//if (IsDead) return;
	//if (bBlockHit) return;
	//if (DirectHit) return;

	//switch (OverlappingActor)
	//{
	//case OverlappingActorType::Item:
	//	//if (::IsValid(CurrentItem) && ::IsValid(InventoryManager))
	//	//{
	//	//	InventoryManager->OnItemOverlapped.Broadcast(true, CurrentItem);
	//	//}

	//	break;

	//case OverlappingActorType::BonFire:
	//	TArray<AActor*> EnemiesToReset;
	//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AABEnemyCharacter::StaticClass(), EnemiesToReset);

	//	if (OverlappedActor != nullptr)
	//	{
	//		if (ABonfire* Bonfire = Cast<ABonfire>(OverlappedActor))
	//		{
	//			//Bonfire->BonfireReset = true;
	//			//Bonfire->OnBonfireReset.Broadcast(EnemiesToReset);
	//		}
	//	}

	//	for (auto const& Enemy : EnemiesToReset)
	//	{
	//		if (AABEnemyCharacter* EnemyCharacter = Cast<AABEnemyCharacter>(Enemy))
	//		{
	//			if (EnemyCharacter->GetIsDead())
	//			{
	//				EnemyCharacter->SetIsDead(false);
	//				EnemyCharacter->SetActorEnableCollision(true);
	//			}

	//			EnemyCharacter->SetIsBeingTargeted(false);
	//			EnemyCharacter->SetActorTransform(EnemyCharacter->GetDefaultTransform());

	//			if (EnemyCharacter->DoesEnemyAnimInstanceExist())
	//			{
	//				EnemyCharacter->GetEnemyAnimInstance()->ResetAnimInstance();
	//			}

	//			EnemyCharacter->GetController<AABAIController>()->BrainComponent->StopLogic(TEXT(""));
	//			EnemyCharacter->GetController<AABAIController>()->BrainComponent->RestartLogic();

	//			EnemyCharacter->SetActorLocation(EnemyCharacter->GetDefaultTransform().GetLocation());
	//			EnemyCharacter->SetActorRotation(EnemyCharacter->GetDefaultTransform().GetRotation());

	//			EnemyCharacter->GetCapsuleComponent()->SetCollisionProfileName(TEXT("TestCharacter"));
	//		}
	//	}

	//	LockOnPressed = false;
	//	HasTarget = false;
	//	GetCharacterMovement()->bOrientRotationToMovement = true;
	//	bUseControllerRotationYaw = false;
	//	Target = nullptr;
	//	GetWorldTimerManager().ClearTimer(LockOnTimer);
	//	GetWorldTimerManager().ClearTimer(ResetCameraTimer);

	//	break;
	//}
	if (IsDead) return;
	if (IsSpawning) return;

	InteractionComponent->Interact();
}

void ABaseCharacter::LockOn_ResetCamera()
{
	if (IsDead) return;
	if (IsSpawning) return;

	LockOnPressed = !LockOnPressed;

	AActor* PrevTarget = Target;

	if (LockOnPressed)
	{
		if (FindTarget())
		{
			LockOnDelegate = FTimerDelegate::CreateUObject(this, &ABaseCharacter::TargetEnemy);
			GetWorldTimerManager().SetTimer(LockOnTimer, LockOnDelegate, GetWorld()->GetDeltaSeconds(), true);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(ResetCameraTimer);

			YawValue = GetActorRotation().Yaw;

			GetWorldTimerManager().SetTimer(ResetCameraTimer, FTimerDelegate::CreateLambda([&]()
				{
					FRotator TargetRot = FRotator(-20, YawValue, 0);
					GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), CameraRotationSpeed));

					if ((GetControlRotation() - TargetRot).IsNearlyZero(.1))
					{
						LockOnPressed = false;
						GetWorldTimerManager().ClearTimer(ResetCameraTimer);
					}
				}), GetWorld()->GetDeltaSeconds(), true);
		}
	}
	else
	{
		HasTarget = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		if (!TargetCharacters.IsEmpty())
		{
			for (auto const& Characters : TargetCharacters)
			{
				Characters->SetIsBeingTargeted(false);
			}
		}

		TargetCharacters.Empty();
		Target = nullptr;

		GetWorldTimerManager().ClearTimer(LockOnTimer);
		GetWorldTimerManager().ClearTimer(ResetCameraTimer);
	}
}

void ABaseCharacter::ToggleInventory()
{
	if (IsDead) return;
	if (!PlayerController || !InventoryManager) return;
	if (IsSpawning) return;

	if (!PlayerController->IsInventoryUIOpen())
	{
		PlayerController->OpenInventoryUI(InventoryManager);
	}
	else
	{
		PlayerController->CloseInventoryUI();
	}
}

void ABaseCharacter::ToggleEquipment()
{
	if (IsDead) return;
	if (!PlayerController || !InventoryManager) return;
	if (IsSpawning) return;

	if (PlayerController->IsEquipItemUIOpen())
	{
		PlayerController->CloseEquipItemUI(InventoryManager);
		return;
	}

	if (!PlayerController->IsEquipmentUIOpen())
	{
		PlayerController->OpenEquipmentUI(InventoryManager);
	}
	else
	{
		PlayerController->CloseEquipmentUI();
	}
}

void ABaseCharacter::CloseWidget()
{
	AABPlayerController* ABPC = Cast<AABPlayerController>(Controller);
	if (!ABPC) return;

	if (ABPC->IsShopUIOpen())
	{
		ABPC->CloseShopUI();
	}
	else if (ABPC->IsBonfireUIOpen())
	{
		ABPC->CloseBonfireUI();
	}
	else if (ABPC->IsLevelUpUIOpen())
	{
		ABPC->CloseLevelUpUI();
		return;
	}
	else if (ABPC->IsInventoryUIOpen())
	{
		ABPC->CloseInventoryUI();
	}
	else if (ABPC->IsEquipmentUIOpen())
	{
		ABPC->CloseEquipmentUI();
	}
	else if (ABPC->IsEquipItemUIOpen())
	{
		if (!InventoryManager) return;

		ABPC->CloseEquipItemUI(InventoryManager);
	}
}

void ABaseCharacter::HotkeyIndexUp()
{
	if (!PlayerHUDWidget) return;

	PlayerHUDWidget->NextHotkey(InventoryManager);
}

void ABaseCharacter::HotkeyIndexDown()
{
	if (!PlayerHUDWidget) return;

	PlayerHUDWidget->PrevHotkey(InventoryManager);
}

void ABaseCharacter::UseHotkey(int32 SlotIndex)
{
	if (!PlayerHUDWidget) return;

	TMap<EEquipmentSlotType, FInventoryItem> Items = InventoryManager->GetAllHotkeyItems();
	if (Items.Num() <= 0) return;

	FInventoryItem* Item = nullptr;

	if (SlotIndex == 1)
	{
		Item = Items.Find(EEquipmentSlotType::Hotkey_1);
	}
	else if (SlotIndex == 2)
	{
		Item = Items.Find(EEquipmentSlotType::Hotkey_2);
	}
	else if (SlotIndex == 3)
	{
		Item = Items.Find(EEquipmentSlotType::Hotkey_3);
	}
	else if (SlotIndex == 4)
	{
		Item = Items.Find(EEquipmentSlotType::Hotkey_4);
	}
	else if (SlotIndex == 5)
	{
		Item = Items.Find(EEquipmentSlotType::Hotkey_5);
	}

	if (!Item) return;

	FName ID(Item->ItemID.ToString());

	const FConsumableData* Data = InventoryManager->GetConsumableData(ID);
	if (!Data) return;

	Anim->OnItemUsed.BindLambda([this, ID]()
		{
			InventoryManager->UseItem(ID, 1);
			PlayerHUDWidget->RefreshHotkey();
		});

	PlayAnimMontage(Data->UseMontage);
}

void ABaseCharacter::Misc()
{
	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->SaveGameData();
	}
}
#pragma endregion

void ABaseCharacter::ChangeTarget(const FInputActionValue& Value)
{
	if (IsDead) return;
	if (!HasTarget) return;
	if (IsSpawning) return;

	// 적이 화면 기준 왼쪽부터 오른쪽까지 정렬
	TArray<AABEnemyCharacter*> Characters = SortTargetCharactersByRightOrLeft(TargetCharacters);
	float AxisValue = Value.Get<float>();

	if (AxisValue < 0)
	{
		for (int32 i = 0; i < Characters.Num(); i++)
		{
			if (Target == Characters[i])
			{
				if (i != 0)
				{
					Target = Characters[i - 1];
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
	else if (AxisValue > 0)
	{
		for (int32 i = 0; i < Characters.Num(); i++)
		{
			if (Target == Characters[i])
			{
				if (i != (Characters.Num() - 1))
				{
					Target = Characters[i + 1];
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
}

void ABaseCharacter::OnMontageStarted(UAnimMontage* Montage)
{
	IsAbleToMove = false;
}

void ABaseCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == Anim->LightAttackMontage)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Light Attack Montage End"));

		IsLightAttacking = false;
		CanNextLightAttackCombo = false;
		CurrentLightAttackCombo = 0;

		return;
	}

	if (Montage == Anim->KnockDownMontage)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Knock Down Montage End"));

		CharacterStatManager->RecoverStamina();
		KnockDown = false;

		return;
	}

	if (Montage == Anim->BlockHitMontage)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Block Hit Montage End"));

		bBlockHit = false;

		return;
	}

	if (Montage == Anim->HitMontage)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Hit Montage End"));

		DirectHit = false;

		return;
	}

	if (Montage == Anim->Player_ExecutionMontage)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Execution Montage End"));

		IsExecuting = false;

		return;
	}

	if ((Montage == Anim->RollMontage || Montage == Anim->DodgeMontage) && !bInterrupted)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Roll Montage End"));

		IsRolling = false;
		IsReRollInputOn = false;
		CanReRoll = false;
		IsAbleToMove = true;

		return;
	}

	if (Montage == Anim->SpawnMontage)
	{
		IsSpawning = false;

		return;
	}
}

void ABaseCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UClass* OverlapClass = OtherActor->GetClass();
	OverlappedActor = OtherActor;

	if (OverlapClass == ABonfire::StaticClass())
	{
		OverlappingActor = OverlappingActorType::BonFire;
		CurrentItem = nullptr;

		OnActorBeginOverlap.Broadcast();
	}
	else if (OverlapClass == ABaseItem::StaticClass())
	{
		OverlappingActor = OverlappingActorType::Item;

		if (ABaseItem* Item = Cast<ABaseItem>(OtherActor))
		{
			CurrentItem = Item;
		}

		OnActorBeginOverlap.Broadcast();

		//if (::IsValid(InventoryManager)) InventoryManager->OnItemOverlapped.Broadcast(false, nullptr);
	}
	else
	{
		OverlappingActor = OverlappingActorType::None;
		CurrentItem = nullptr;
	}
}

void ABaseCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingActor = OverlappingActorType::None;
	CurrentItem = nullptr;

	OnActorEndOverlap.Broadcast();
}

void ABaseCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 무기 콜라이더와 오버랩 된 액터가 BaseCharacter이면 return
	if (OtherActor == this) return;

	if (AABEnemyCharacter* TargetEnemy = Cast<AABEnemyCharacter>(OtherActor))
	{
		if (TargetEnemy->GetIsDead()) return;

		// 데미지를 두 번 받지 않을 수 있게 방지
		if (!TargetEnemy->IsAttacked && !AttackedEnemies.Contains(TargetEnemy))
		{
			AttackedEnemies.Add(TargetEnemy);
			TargetEnemy->IsAttacked = true;

			// CharacterStatComponent에서 플레이어의 기본 공격력과 무기 공격력을 합산
			int32 Damage = CharacterStatManager->GetOverallDamage();

			// 적에게 데미지 전달
			UGameplayStatics::ApplyDamage(TargetEnemy, Damage, GetController(), this, UDamageType::StaticClass());
		}
	}
}

void ABaseCharacter::SetStaminaRegainTimer()
{
	if (GetWorldTimerManager().IsTimerActive(StaminaRegainTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StaminaRegainTimerHandle);
	}

	GetWorldTimerManager().SetTimer(StaminaRegainTimerHandle, this, &ABaseCharacter::RegainStamina, LastDeltaTime, true, StaminaRegainDelaySeconds);
}

void ABaseCharacter::RegainStamina()
{
	// 스태미나 회복 중
	RegainingStamina = true;

	// ChracterStatComponent에서 수치 불러오기
	float CurrentStamina = CharacterStatManager->GetCurrentStamina();
	float MaxStamina = CharacterStatManager->GetMaxStamina();

	// 스태미나가 최대일 때
	if (CurrentStamina >= MaxStamina)
	{
		// 현재 스태미나를 최대로
		CurrentStamina = MaxStamina;

		// 스태미나 회복 종료
		RegainingStamina = false;
		GetWorldTimerManager().ClearTimer(StaminaRegainTimerHandle);
	}
	else
	{
		// 스태미나 회복
		// 가드를 하고 있을 때, 회복 속도 반감
		CurrentStamina += LastDeltaTime * StaminaRegainMultiplier * (IsBlocking ? 0.5f : 1.0f);
	}

	if (CurrentStamina >= MaxStamina * 0.1f && IsExhausted)
	{
		IsExhausted = false;
	}

	// CharacterStatManager에 수치 업데이트
	CharacterStatManager->CurrentStamina = CurrentStamina;
	CharacterStatManager->OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void ABaseCharacter::UpdatePlayerTransform()
{
	if (IsDead) return;

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->PlayerTransform = GetActorTransform();
		GI->CurrentSave->bHasValidPlayerTransform = true;

		GI->MarkDirty();
	}
}

void ABaseCharacter::LightAttackStartComboState()
{
	CanNextLightAttackCombo = true;
	IsLightAttackComboInputOn = false;

	if (!FMath::IsWithinInclusive<int32>(CurrentLightAttackCombo, 0, MaxLightAttackCombo - 1)) return;

	CurrentLightAttackCombo = FMath::Clamp<int32>(CurrentLightAttackCombo + 1, 1, MaxLightAttackCombo);
}

void ABaseCharacter::HeavyAttackStartComboState()
{
	CanNextHeavyAttackCombo = true;
	IsHeavyAttackComboInputOn = false;

	if (!FMath::IsWithinInclusive<int32>(CurrentHeavyAttackCombo, 0, MaxHeavyAttackCombo - 1)) return;

	CurrentHeavyAttackCombo = FMath::Clamp<int32>(CurrentHeavyAttackCombo + 1, 1, MaxHeavyAttackCombo);
}

void ABaseCharacter::AttackEndComboState()
{
	IsLightAttackComboInputOn = false;
	IsHeavyAttackComboInputOn = false;

	CanNextLightAttackCombo = false;
	CanNextHeavyAttackCombo = false;

	CurrentLightAttackCombo = 0;
	CurrentHeavyAttackCombo = 0;
}

// 구르기 (회피)
void ABaseCharacter::PlayRollMontage()
{
	IsRolling = true;
	CanReRoll = false;

	// 스태미나 차감 (테스트 용으로 5 차감)
	SpendStamina(5);

	// 이동 키 (W, A, S, D)를 누르지 않았을 때 뒤로 회피 몽타주 실행
	if (!IsMoveKeyPressed)
	{
		Anim->Montage_Play(Anim->DodgeMontage);
		return;
	}

	// 타겟팅이 되어있을 때
	if (HasTarget)
	{
		// 구르기 몽타주 실행
		Anim->Montage_Play(Anim->RollMontage);

		// 이동 키에 따른 이동 각도를 계산하여 애니메이션 실행
		Anim->Montage_JumpToSection(GetInputDirectionName(Axis2D), Anim->RollMontage);
	}
	else
	{
		// 몽타주 실행
		Anim->Montage_Play(Anim->RollMontage);

		// 몽타주가 끝나고 바로 다시 구르기를 실행할 경우를 대비해 이동하는 방향에 따라 플레이어 회전
		ChangeAnimMontageDirection();
	}
}

void ABaseCharacter::TargetEnemy()
{
	// 적을 찾고 찾으면 가장 가까이 있는 적을 Target으로 저장, 전체 적을 TargetCharacters에 저장.
	// 적을 찾으면 return true
	if (FindTarget())
	{
		if (AABEnemyCharacter* TargetCharacter = Cast<AABEnemyCharacter>(Target))
		{
			for (auto const& Character : TargetCharacters)
			{
				// 타겟이 되었는지 세팅
				if (TargetCharacter == Character)
				{
					Character->SetIsBeingTargeted(true);
				}
				else
				{
					Character->SetIsBeingTargeted(false);
				}
			}

			// 적이 죽으면 TargetCharacters에서 Target 제거 후 다음 적을 타겟팅
			if (TargetCharacter->GetIsDead())
			{
				if (!TargetCharacters.IsEmpty())
				{
					for (int32 i = 0; i < TargetCharacters.Num(); i++)
					{
						if (TargetCharacter == TargetCharacters[i])
						{
							if (i == 0)
							{
								LockOnPressed = false;
								HasTarget = false;
								GetCharacterMovement()->bOrientRotationToMovement = true;
								bUseControllerRotationYaw = false;
								Target = nullptr;
								GetWorldTimerManager().ClearTimer(LockOnTimer);

								break;
							}

							if (i == TargetCharacters.Num() - 1)
							{
								Target = TargetCharacters[i - 1];
							}
							else
							{
								Target = TargetCharacters[i + 1];
							}
						}
					}
				}
				else
				{
					LockOnPressed = false;
					HasTarget = false;
					GetCharacterMovement()->bOrientRotationToMovement = true;
					bUseControllerRotationYaw = false;
					Target = nullptr;
					GetWorldTimerManager().ClearTimer(LockOnTimer);
				}

				TargetCharacter->SetIsBeingTargeted(false);
			}
			else
			{
				HasTarget = true;

				// 카메라 적에 고정
				TargetCamera(Target->GetActorLocation());

				if (!QuickTurn && !IsSprinting)
				{
					GetCharacterMovement()->bOrientRotationToMovement = false;
					bUseControllerRotationYaw = true;
				}
			}
		}
	}
}

bool ABaseCharacter::FindTarget()
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel
	(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(MaxTargetSearchDistance),
		Params
	);

	if (bResult)
	{
		for (auto const& HitResult : HitResults)
		{
			if (AABEnemyCharacter* Character = Cast<AABEnemyCharacter>(HitResult.GetComponent()->GetOwner()))
			{
				APlayerController* PC = GetController<APlayerController>();

				FVector2D ScreenLocation;
				int32 ViewportX;
				int32 ViewportY;

				// 적이 현재 화면 안에 있는지 확인
				bool OnScreen = PC->ProjectWorldLocationToScreen(Character->GetActorLocation(), ScreenLocation);
				PC->GetViewportSize(ViewportX, ViewportY);

				if ((ScreenLocation.X > 0.0) && (ScreenLocation.Y > 0.0) && (ScreenLocation.X < ViewportX) && (ScreenLocation.Y < ViewportY) && OnScreen)
				{
					if (!TargetCharacters.Contains(Character))
					{
						TargetCharacters.Add(Character);
					}
				}
				else
				{
					if (TargetCharacters.Contains(Character))
					{
						Character->SetIsBeingTargeted(false);
						TargetCharacters.Remove(Character);
					}
				}

				if (TargetCharacters.Num() > HitResults.Num())
				{
					for (auto const& TargetCharacter : TargetCharacters)
					{
						Character->SetIsBeingTargeted(false);
					}

					TargetCharacters.Empty();
				}
			}
		}

		if (!TargetCharacters.IsEmpty())
		{
			TargetCharacters = SortTargetCharacters(TargetCharacters);

			if (Target == nullptr)
			{
				Target = TargetCharacters[0];
			}
			else
			{
				if (!LockOnPressed)
				{
					Target = TargetCharacters[0];
				}
			}

			for (auto const& TargetCharacter : TargetCharacters)
			{
				if (Target != TargetCharacter)
				{
					TargetCharacter->SetIsBeingTargeted(false);
				}
			}
		}
		else
		{
			Target = nullptr;
		}
	}
	else
	{
		if (GetWorldTimerManager().IsTimerActive(LockOnTimer))
		{
			TargetStatusChanged = true;
		}

		for (auto const& TargetCharacter : TargetCharacters)
		{
			TargetCharacter->SetIsBeingTargeted(false);
		}

		TargetCharacters.Empty();
		Target = nullptr;
		HasTarget = false;

		LockOnPressed = false;

		GetWorldTimerManager().ClearTimer(LockOnTimer);
	}

	TargetInRange = (bResult && !TargetCharacters.IsEmpty());

	FColor Color = TargetInRange ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), GetActorLocation(), MaxTargetSearchDistance, 16, Color, false, 0.01f);

	return TargetInRange;
}

void ABaseCharacter::TargetCamera(FVector TargetLocation)
{
	FRotator PlayerRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	PlayerRotation.Pitch = -20.0f;
	GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), PlayerRotation, GetWorld()->GetDeltaSeconds(), CameraRotationSpeed));
}

TArray<AABEnemyCharacter*> ABaseCharacter::SortTargetCharacters(TArray<AABEnemyCharacter*> TargetCharacter)
{
	TArray<AABEnemyCharacter*> PriorityTargets;
	TArray<AABEnemyCharacter*> OtherTargets;
	TArray<AABEnemyCharacter*> EnemyTargets;

	float CharacterFacingRadius = 15.0f;

	float CharacterYaw = FMath::Abs(GetActorRotation().Yaw);
	float TargetYaw;

	FVector CharacterLocation = GetActorLocation();

	for (auto const& Enemy : TargetCharacter)
	{
		if (GetDistanceTo(Enemy) <= 300.0f)
		{
			TargetYaw = FMath::Abs(UKismetMathLibrary::FindLookAtRotation(CharacterLocation, Enemy->GetActorLocation()).Yaw);
			if (UKismetMathLibrary::InRange_FloatFloat(CharacterYaw, TargetYaw - CharacterFacingRadius, TargetYaw + CharacterFacingRadius))
			{
				PriorityTargets.Add(Enemy);

			}
			else
			{
				OtherTargets.Add(Enemy);
			}
		}
		else
		{
			OtherTargets.Add(Enemy);
		}
	}

	if (!PriorityTargets.IsEmpty())
	{
		if (PriorityTargets.Num() == 1)
		{
			EnemyTargets.Add(PriorityTargets[0]);

			PriorityTargets = OtherTargets;
		}
		else
		{
			PriorityTargets.Sort([this](const AABEnemyCharacter& A, const AABEnemyCharacter& B)
				{
					float DistanceToA = GetDistanceTo(A.GetParentActor());
					float DistanceToB = GetDistanceTo(B.GetParentActor());

					return DistanceToA < DistanceToB;
				});

			EnemyTargets.Add(PriorityTargets[0]);

			if (!PriorityTargets.IsEmpty())
			{
				for (auto const& Enemy : PriorityTargets)
				{
					if (PriorityTargets[0] != Enemy)
					{
						OtherTargets.Add(Enemy);
					}
				}
			}
		}

		OtherTargets.Sort([this](const AABEnemyCharacter& A, const AABEnemyCharacter& B)
			{
				float CenterA = CheckTargetCloseToCenter(A.GetActorLocation());
				float CenterB = CheckTargetCloseToCenter(B.GetActorLocation());

				return CenterA > CenterB;
			});

		EnemyTargets.Append(OtherTargets);
	}
	else
	{
		EnemyTargets = OtherTargets;
		EnemyTargets.Sort([this](const AABEnemyCharacter& A, const AABEnemyCharacter& B)
			{
				float CenterA = CheckTargetCloseToCenter(A.GetActorLocation());
				float CenterB = CheckTargetCloseToCenter(B.GetActorLocation());

				return CenterA > CenterB;
			});
	}

	return EnemyTargets;
}

TArray<AABEnemyCharacter*> ABaseCharacter::SortTargetCharactersByRightOrLeft(TArray<AABEnemyCharacter*> TargetCharacter)
{
	TargetCharacter.Sort([this](const AABEnemyCharacter& A, const AABEnemyCharacter& B)
		{
			float RightLeftA = CheckTargetOnRightOrLeft(A.GetActorLocation());
			float RightLeftB = CheckTargetOnRightOrLeft(B.GetActorLocation());

			return RightLeftA < RightLeftB;
		});

	return TargetCharacter;
}

float ABaseCharacter::CheckTargetCloseToCenter(FVector TargetActorLocation)
{
	FVector CameraForwardVector = Camera->GetForwardVector();
	FVector PlayerToTargetRotationXVector = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActorLocation).Vector();

	float DotResult = FVector::DotProduct(CameraForwardVector, PlayerToTargetRotationXVector);

	return DotResult;
}

float ABaseCharacter::CheckTargetOnRightOrLeft(FVector TargetActorLocation)
{
	APlayerController* PC = GetController<APlayerController>();

	FVector2D ScreenLocation;
	int32 ViewportXSize;
	int32 ViewportYSize;

	PC->ProjectWorldLocationToScreen(TargetActorLocation, ScreenLocation);
	PC->GetViewportSize(ViewportXSize, ViewportYSize);

	float Result = ScreenLocation.X - ViewportXSize;

	return Result;

}

void ABaseCharacter::ChangeAnimMontageDirection()
{
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Axis2D.X, Axis2D.Y));
	float Yaw = GetControlRotation().Yaw + Angle;
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0.0f, Yaw, 0.0f), GetWorld()->GetDeltaSeconds(), 100.0f));
}

FName ABaseCharacter::GetInputDirectionName(FVector2D InputAxis)
{
	float InputAngle = FMath::RadiansToDegrees(FMath::Atan2(InputAxis.X, InputAxis.Y));

	if (InputAngle <= 157.5f && InputAngle >= 112.5f)
	{
		return TEXT("135");
	}
	else if (InputAngle < 112.5f && InputAngle > 67.5f)
	{
		return TEXT("90");
	}
	else if (InputAngle <= 67.5f && InputAngle >= 22.5f)
	{
		return TEXT("45");
	}
	else if (InputAngle < 22.5f && InputAngle > -22.5f)
	{
		return TEXT("0");
	}
	else if (InputAngle <= -22.5f && InputAngle >= -67.5f)
	{
		return TEXT("-45");
	}
	else if (InputAngle < -67.5f && InputAngle > -112.5f)
	{
		return TEXT("-90");
	}
	else if (InputAngle <= -112.5f && InputAngle >= -157.5f)
	{
		return TEXT("-135");
	}
	else if (InputAngle < -157.5f || InputAngle > 157.5f)
	{
		return TEXT("-180");
	}
	else
	{
		return TEXT("0");
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 방어 스탯에서 데미지 경감
	FinalDamage = FinalDamage * (100.0f / (100.0f + (float)CharacterStatManager->GetOverallDefence()));

	// 공격을 가드하고 있는지에 따라 데미지 경감
	if (IsBlocking)
	{
		FinalDamage = FinalDamage * (GuardDamageReductionPercentage / 100.0f);
	}

	// 스탯에 적용
	CharacterStatManager->AddHP(-FinalDamage);
	if (CharacterStatManager->CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return FinalDamage;
}