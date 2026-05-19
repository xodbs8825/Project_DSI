#pragma once

#include "EngineMinimal.h"
#include "Engine/DamageEvents.h"
#include "InputActionValue.h"
#include "ABEnemyCharacter.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "BaseCharacter.generated.h"

struct FWeaponData;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnActorBeginOverlap);
DECLARE_MULTICAST_DELEGATE(FOnActorEndOverlap);
DECLARE_MULTICAST_DELEGATE(FOnNewLevelSet);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBlockHit, bool, bParrySucceeded, int32, StaminaDamage, AABEnemyCharacter*, Enemy);
DECLARE_MULTICAST_DELEGATE(FOnDirectHit);

class UInteractionComponent;
class UInputAction;

UCLASS()
class PROJECT_DSI_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

#pragma region InputAction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputMappingContext* IMC_UI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* LockOn_ResetCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* ChangeTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* EquipmentAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* UseHotkey_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* UseHotkey_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* UseHotkey_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* UseHotkey_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* UseHotkey_5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* MiscAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* CloseUIWidget;
#pragma endregion

protected:
	void Move(const FInputActionValue& Value);
	void MoveEnd();

	bool IsMoveKeyPressed = false;

	void SprintStarted(const FInputActionInstance& Instance);
	void SprintTriggered(const FInputActionInstance& Instance);
	void SprintEnd();

	void Look(const FInputActionValue& Value);
	void ChangeTarget(const FInputActionValue& Value);

	virtual void Jump() override;

	UFUNCTION()
	void HeavyAttack(const FInputActionInstance& Instance);
	void HeavyAttackEnd();

	void Roll();

	void Interact();
	void LockOn_ResetCamera();

	void ToggleInventory();
	void ToggleEquipment();
	void CloseWidget();

	void HotkeyIndexUp();
	void HotkeyIndexDown();
	void UseHotkey(int32 SlotIndex);

	void Misc();

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetControlMode(int32 ControlMode);

	UFUNCTION()
	void UIOpened(AABPlayerController* PC);

	UFUNCTION()
	void UIClosed(AABPlayerController* PC);

	UFUNCTION()
	void BlockHit(bool bParrySucceeded, int32 PostureDamage, AABEnemyCharacter* Enemy);

protected:
	FRotator DefaultMeshRotator;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

public:
	UPROPERTY(VisibleInstanceOnly, Category = UI)
	class UPlayerHUD* PlayerHUDWidget;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FOnAttackEndDelegate OnAttackEnd;
	void LightAttack();

	void SetIsAttacking(bool bValue) { IsLightAttacking = bValue; }

	bool GetTargetStatusChanged() { return TargetStatusChanged; }
	void SetTargetStatusChanged(bool bValue) { TargetStatusChanged = bValue; }

	void SetMaxLightAttackMontageSection(int32 iValue) { MaxLightAttackCombo = iValue; }
	void SetMaxHeavyAttackMontageSection(int32 iValue) { MaxHeavyAttackCombo = iValue; }

	void SetWeapon(const FWeaponData* WeaponData);
	void ClearWeapon();

	void SpendStamina(float Stamina);

	bool CheckWouldPlayerDie(float ExpectedDamage);

	void OnDeath();

	UFUNCTION(BlueprintCallable)
	void Respawn();
	void SpawnPlayer();
	void DropSouls();

	class ABonfire* FindBonfireByID(FName BonfireID);

	void RegisterEnemies(AABEnemyCharacter* Enemy);
	void ResetEnemies();

public:
	TArray<TWeakObjectPtr<AABEnemyCharacter>> Enemies;

	bool IsSpawning = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASoulsDropActor> SoulsDropClass;

	UPROPERTY(BlueprintReadOnly)
	class AWeaponBaseActor* WeaponActor;

	bool LockOnPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Targeting, Meta = (AllowPrivateAccess = true))
	bool HasTarget;

	FOnActorBeginOverlap OnActorBeginOverlap;
	FOnActorEndOverlap OnActorEndOverlap;
	FOnNewLevelSet OnNewLevelSet;

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterStatus")
	class UABCharacterStatComponent* CharacterStatManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InventoryComponent")
	class UInventoryManager* InventoryManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere)
	class ABaseItem* CurrentItem;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Status, Meta = (AllowPrivateAccess = true))
	bool IsDead;

protected:
	class AABPlayerController* PlayerController;

private:
	void LightAttackStartComboState();
	void HeavyAttackStartComboState();
	void AttackEndComboState();

	void PlayRollMontage();

	void TargetEnemy();
	bool FindTarget();
	void TargetCamera(FVector TargetLocation);

	TArray<AABEnemyCharacter*> SortTargetCharacters(TArray<AABEnemyCharacter*> TargetCharacter);
	TArray<AABEnemyCharacter*> SortTargetCharactersByRightOrLeft(TArray<AABEnemyCharacter*> TargetCharacter);
	float CheckTargetCloseToCenter(FVector TargetActorLocation);
	float CheckTargetOnRightOrLeft(FVector TargetActorLocation);

	void ChangeAnimMontageDirection();
	FName GetInputDirectionName(FVector2D InputAxis);

	UFUNCTION()
	void OnMontageStarted(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool IsAttacked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool IsBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool IsAbleToParry = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool ParrySucceeded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	float GuardDamageReductionPercentage;

	FOnBlockHit OnBlockHit;
	FOnDirectHit OnDirectHit;

	float LastDeltaTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture Regain", Meta = (AllowPrivateAccess = true))
	float StaminaRegainMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture Regain", Meta = (AllowPrivateAccess = true))
	bool RegainingStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture Regain", Meta = (AllowPrivateAccess = true))
	float StaminaRegainDelaySeconds = 2.0f;

	FTimerHandle StaminaRegainTimerHandle;

	FTimerHandle PlayerTransformUpdateTimer;

public:
	void SetStaminaRegainTimer();
	void RegainStamina();

	void UpdatePlayerTransform();

private:
	enum OverlappingActorType
	{
		None,
		Item,
		BonFire
	};

	FVector2D Axis2D;

	float UpAxis;
	float RightAxis;
	float YawValue;

	FVector ForwardVec;
	FVector RightVec;

	OverlappingActorType OverlappingActor;

	FTimerHandle SprintTimer;
	FTimerHandle LockOnTimer;
	FTimerHandle ResetCameraTimer;

	FTimerDelegate LockOnDelegate;

	bool TargetInRange;
	bool TargetStatusChanged = false;

	bool HeavyAttackPressed = false;

	FTimerHandle RespawnTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool KnockDown = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool QuickTurn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, Meta = (AllowPrivateAccess = true))
	bool IsSprinting = false;

	bool IsHoldingSprint = false;
	bool IsExhausted = false;

	float HoldTime = 0.0f;
	float HoldThreshold = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Targeting, Meta = (AllowPrivateAccess = true))
	TArray<AABEnemyCharacter*> TargetCharacters;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Interact, Meta = (AllowPrivateAccess = true))
	AActor* OverlappedActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsLightAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsHeavyAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextLightAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsLightAttackComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentLightAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxLightAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextHeavyAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsHeavyAttackComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentHeavyAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxHeavyAttackCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	TArray<AABEnemyCharacter*> AttackedEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool IsAbleToAttack = true;

	UPROPERTY()
	class UBaseAnimInstance* Anim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", Meta = (AllowPrivateAccess = true))
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", Meta = (AllowPrivateAccess = true))
	float MaxTargetSearchDistance;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Targeting", Meta = (AllowPrivateAccess = true))
	float CameraRotationSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Targeting", Meta = (AllowPrivateAccess = true))
	bool IsBeingTargeted;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Reset", Meta = (AllowPrivateAccess = true))
	FTransform DefaultTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool bBlockHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	bool DirectHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float CharacterRotationSpeed = 100.0f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float ViewAngle = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool IsRolling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool CanReRoll = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool IsReRollInputOn = false;

	bool IsInIFrame = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool IsAbleToRoll = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool IsAbleToMove = true;

	AABEnemyCharacter* EnemyBeingExecuted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsOverlappingInteractableObject = false;

	float PostureDamage = 3.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	float ExecutionRange = 200.0f;

	bool IsExecuting;
};
