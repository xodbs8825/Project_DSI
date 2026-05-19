// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "ArmorBaseItem.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

struct FWeaponData;

class ASoulsDropActor;

DECLARE_DELEGATE_OneParam(FOnSoulsChaged, int32);
DECLARE_DELEGATE_TwoParams(FOnSoulsDropped, int32, bool);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSetHPStaminaSize, float, CurrentHP, float, VigorMaxHP, float, CurrentStamina, float, EnduranceMaxStamina);

UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	Vigor,
	Endurance,
	Strength
};

USTRUCT(BlueprintType)
struct FStatSoftCapRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EPlayerStatType StatType;

	UPROPERTY(EditAnywhere)
	int32 Threshold;

	UPROPERTY(EditAnywhere)
	float Multiplier;
};

USTRUCT(BlueprintType)
struct FDefenceSoftCap : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Threshold;

	UPROPERTY(EditAnywhere)
	float Multiplier;
};

USTRUCT(BlueprintType)
struct FSoftCapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Threshold;

	UPROPERTY(EditAnywhere)
	float Multiplier;
};

USTRUCT(BlueprintType)
struct FPlayerStatData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Vigor = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Endurance = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Strength = 5;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_DSI_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetDamage(float NewDamage);
	void SetHP(float NewHP);

	float GetAttackDamage() const;

	void SetWeaponData(const FWeaponData* WeaponData);
	void ClearWeaponData();

	void SetArmorData(const FArmorData* ArmorData);
	void ClearArmorData(EArmorType ArmorType);

	UFUNCTION(BlueprintCallable)
	void AddHP(float HP);

	void SetCurrentHP(float HP);

	float GetCurrentStamina() const { return CurrentStamina; }
	void AddStamina(float AddedStamina);
	void SetCurrentStamina(float NewStamina);

	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponDamage() const { return WeaponDamage; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponDamage(int32 NewWeaponDamage) { WeaponDamage = NewWeaponDamage; }

	void CalculateOverallArmorDefence();

	UFUNCTION(BlueprintCallable)
	int32 GetOverallDamage() const { return PlayerDamage + WeaponDamage; }

	void SetWeaponStaminaCost(int32 Cost) { WeaponStaminaCost = Cost; }
	int32 GetWeaponStaminaCost() { return WeaponStaminaCost; }
	int32 GetOverallStaminaCost() { return WeaponStaminaCost + BaseStaminaCost; }

	UFUNCTION(BlueprintCallable)
	int32 GetOverallArmorDefence() const { return OverallArmorDefence; }

	UFUNCTION(BlueprintCallable)
	int32 GetOverallDefence() const { return Defence + OverallArmorDefence; }

	int32 GetSouls() { return Soul; }
	void AddSouls(int32 Value);
	void PickUpSouls(int32 Value);
	void ResetSouls();
	void DropAllSouls(ASoulsDropActor* DroppedSoulsActor);

	void InitializeStat();
	void RecoverFull();
	void RecoverHP();
	void RecoverStamina();

	int32 GetLevelUpCost() const;
	bool CanLevelUp() const;
	bool TryLevelUp(const FPlayerStatData& AddedStats);

	int32 CalculateLevelUpCost(int32 AddedLevel) const;
	void RecalculateDerivedStat();

	int32 CalculateDefence(int32 CurrentLevel, int32 CurrentStrength);

	void LoadSoftCaps();
	float ApplySoftCap(int32 StatValue, const TArray<FSoftCapData>& SoftCaps);

	float GetMaxVigorHP();
	float GetMaxEnduranceStamina();

	void ApplyStatData(const FPlayerStatData& NewStatData);

protected:
	int32 WeaponDamage = 0;

	int32 HeadArmorDefence = 0;
	int32 BodyArmorDefence = 0;
	int32 GloveArmorDefence = 0;
	int32 LegArmorDefence = 0;
	int32 FootArmorDefence = 0;

	int32 OverallArmorDefence = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 BaseStaminaCost = 0;

	int32 WeaponStaminaCost = 0;

public:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 Soul = 0;

	UPROPERTY(BlueprintReadOnly)
	FPlayerStatData PlayerStat;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHP;

	UPROPERTY(BlueprintReadOnly)
	float MaxHP;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int32 PlayerDamage;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int32 Defence;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float CurrentStamina;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly)
	int32 DroppedSouls = 0;

	class ASoulsDropActor* LastDroppedSoulsActor;

	FOnSoulsChaged OnSoulsChanged;
	FOnSoulsDropped OnSoulsDropped;

	FOnHPChanged OnHPChanged;
	FOnStaminaChanged OnStaminaChanged;

	FOnSetHPStaminaSize OnSetHPStaminaSize;

private:
	UPROPERTY(EditDefaultsOnly)
	UDataTable* SoftCapTable;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* DefenceSoftCapTable;

public:
	UPROPERTY()
	TArray<FSoftCapData> VigorSoftCaps;

	UPROPERTY()
	TArray<FSoftCapData> EnduranceSoftCaps;

	UPROPERTY()
	TArray<FSoftCapData> StrengthSoftCaps;

	UPROPERTY()
	TArray<FDefenceSoftCap> DefenceSoftCaps;
};