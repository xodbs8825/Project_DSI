// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"
#include "WeaponBaseItem.h"
#include "ArmorBaseItem.h"
#include "SoulsDropActor.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	Level = 1;
}

// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (GI && GI->CurrentSave)
	{
		const UDSI_SaveGame* Save = GI->CurrentSave;

		FPlayerStatData Stat;
		Stat.Vigor = Save->VigorLevel;
		Stat.Endurance = Save->EnduranceLevel;
		Stat.Strength = Save->StrengthLevel;

		CurrentHP = Save->HP;
		Soul = Save->Souls;

		ApplyStatData(Stat);

		if (Save->bHasSaveNewlyCreated)
		{
			RecoverHP();

			GI->CurrentSave->HP = CurrentHP;
			GI->CurrentSave->Souls = Soul;

			GI->MarkDirty();

			GI->SaveGameData();
		}
	}
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UABCharacterStatComponent::SetDamage(float NewDamage)
{

}

void UABCharacterStatComponent::SetHP(float NewHP)
{

}

float UABCharacterStatComponent::GetAttackDamage() const
{
	return 0.0f;
}

void UABCharacterStatComponent::SetWeaponData(const FWeaponData* WeaponData)
{
	SetWeaponDamage(WeaponData->BaseDamage);
	SetWeaponStaminaCost(WeaponData->StaminaCost);
}

void UABCharacterStatComponent::ClearWeaponData()
{
	SetWeaponDamage(0.0f);
	SetWeaponStaminaCost(0.0f);
}

void UABCharacterStatComponent::SetArmorData(const FArmorData* ArmorData)
{
	switch (ArmorData->ArmorType)
	{
	case EArmorType::HeadArmor:
		HeadArmorDefence = ArmorData->BaseDefence;
		break;

	case EArmorType::BodyArmor:
		BodyArmorDefence = ArmorData->BaseDefence;
		break;

	case EArmorType::GloveArmor:
		GloveArmorDefence = ArmorData->BaseDefence;
		break;

	case EArmorType::LegArmor:
		LegArmorDefence = ArmorData->BaseDefence;
		break;

	case EArmorType::FootArmor:
		FootArmorDefence = ArmorData->BaseDefence;
		break;
	}

	CalculateOverallArmorDefence();
}

void UABCharacterStatComponent::ClearArmorData(EArmorType ArmorType)
{
	switch (ArmorType)
	{
	case EArmorType::HeadArmor:
		HeadArmorDefence = 0;
		break;

	case EArmorType::BodyArmor:
		BodyArmorDefence = 0;
		break;

	case EArmorType::GloveArmor:
		GloveArmorDefence = 0;
		break;

	case EArmorType::LegArmor:
		LegArmorDefence = 0;
		break;

	case EArmorType::FootArmor:
		FootArmorDefence = 0;
		break;
	}

	CalculateOverallArmorDefence();
}

void UABCharacterStatComponent::AddHP(float HP)
{
	float Current = CurrentHP;
	Current += HP;

	if (Current <= 0.0f)
	{
		Current = 0.0f;
	}

	if (Current > MaxHP)
	{
		Current = MaxHP;
	}

	CurrentHP = Current;
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UABCharacterStatComponent::SetCurrentHP(float HP)
{
	if (HP <= 0.0f)
	{
		HP = 0.0f;
	}

	if (HP >= MaxHP)
	{
		HP = MaxHP;
	}

	CurrentHP = HP;
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UABCharacterStatComponent::AddStamina(float AddedStamina)
{
	float Current = CurrentStamina;
	Current += AddedStamina;

	if (Current <= 0.0f)
	{
		Current = 0.0f;
	}

	if (Current >= MaxStamina)
	{
		Current = MaxStamina;
	}

	CurrentStamina = Current;

	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UABCharacterStatComponent::SetCurrentStamina(float NewStamina)
{
	if (NewStamina <= 0.0f)
	{
		NewStamina = 0.0f;
	}

	if (NewStamina >= MaxStamina)
	{
		NewStamina = MaxStamina;
	}

	CurrentStamina = NewStamina;
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UABCharacterStatComponent::CalculateOverallArmorDefence()
{
	OverallArmorDefence = HeadArmorDefence + BodyArmorDefence + GloveArmorDefence + LegArmorDefence + FootArmorDefence;
}

void UABCharacterStatComponent::AddSouls(int32 Value)
{
	Soul += Value;
	OnSoulsChanged.Execute(Soul);
}

void UABCharacterStatComponent::PickUpSouls(int32 Value)
{
	AddSouls(Value);
	LastDroppedSoulsActor = nullptr;
	OnSoulsDropped.Execute(0, false);
}

void UABCharacterStatComponent::ResetSouls()
{
	Soul = 0;
	OnSoulsChanged.Execute(0);
}

void UABCharacterStatComponent::DropAllSouls(ASoulsDropActor* DroppedSoulsActor)
{
	if (LastDroppedSoulsActor != nullptr)
	{
		LastDroppedSoulsActor->Destroy();
	}

	LastDroppedSoulsActor = DroppedSoulsActor;

	DroppedSouls = Soul;
	ResetSouls();

	OnSoulsDropped.Execute(DroppedSouls, true);
}

void UABCharacterStatComponent::InitializeStat()
{
	LoadSoftCaps();
	Level = PlayerStat.Vigor + PlayerStat.Endurance + PlayerStat.Strength;
	RecalculateDerivedStat();
	RecoverFull();

	//OnSetHPStaminaSize.Broadcast(CurrentHP, GetMaxVigorHP(), CurrentStamina, GetMaxEnduranceStamina());
}

void UABCharacterStatComponent::RecoverFull()
{
	RecoverHP();
	RecoverStamina();
}

void UABCharacterStatComponent::RecoverHP()
{
	CurrentHP = MaxHP;
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UABCharacterStatComponent::RecoverStamina()
{
	CurrentStamina = MaxStamina;
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

int32 UABCharacterStatComponent::GetLevelUpCost() const
{
	return Level * 100;
}

bool UABCharacterStatComponent::CanLevelUp() const
{
	return Soul >= GetLevelUpCost();
}

bool UABCharacterStatComponent::TryLevelUp(const FPlayerStatData& AddedStat)
{
	// 레벨 업 할 총 레벨 계산
	int32 AddedLevel = AddedStat.Vigor + AddedStat.Endurance + AddedStat.Strength;

	// 레벨 업에 필요한 소울량 계산
	int32 Cost = CalculateLevelUpCost(AddedLevel);

	if (Soul < Cost) return false;

	// 수치 계산 및 업데이트
	Soul -= Cost;
	Level += AddedLevel;

	PlayerStat.Vigor += AddedStat.Vigor;
	PlayerStat.Endurance += AddedStat.Endurance;
	PlayerStat.Strength += AddedStat.Strength;

	// 수치 적용
	RecalculateDerivedStat();

	// 체력과 스태미나가 오르면 차이가 발생하기 때문에 회복
	RecoverFull();

	// HUD에 표시되는 체력과 스태미나 바 크기 조정
	//OnSetHPStaminaSize.Broadcast(CurrentHP, GetMaxVigorHP(), CurrentStamina, GetMaxEnduranceStamina());
	OnSoulsChanged.Execute(Soul);

	UBaseGameInstance* GI = GetWorld()->GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->VigorLevel = PlayerStat.Vigor;
		GI->CurrentSave->EnduranceLevel = PlayerStat.Endurance;
		GI->CurrentSave->StrengthLevel = PlayerStat.Strength;

		GI->CurrentSave->Souls = Soul;

		GI->SaveGameData();
	}

	return true;
}

int32 UABCharacterStatComponent::CalculateLevelUpCost(int32 AddedLevel) const
{
	int32 Cost = 0;
	int32 TempLevel = Level;

	for (int32 i = 0; i < AddedLevel; i++)
	{
		Cost += TempLevel * 100;
		TempLevel++;
	}

	return Cost;
}

void UABCharacterStatComponent::RecalculateDerivedStat()
{
	// 각 스탯 별 소프트캡을 적용한 수치를 가져와서 플레이어의 기본 스탯에 더하기
	float HPFromVigor = ApplySoftCap(PlayerStat.Vigor, VigorSoftCaps);
	float StaminaFromEndurance = ApplySoftCap(PlayerStat.Endurance, EnduranceSoftCaps);
	float DamageFromStrength = ApplySoftCap(PlayerStat.Strength, StrengthSoftCaps);

	// 기본 체력 : 400
	MaxHP = 400 + HPFromVigor;

	// 기본 스태미나 : 100
	MaxStamina = 100 + StaminaFromEndurance;

	// 기본 데미지 : 50
	PlayerDamage = 50 + DamageFromStrength;

	// 방어 스탯은 플레이어의 총 레벨에 기반하기 때문에 다르게 계산
	Defence = CalculateDefence(Level, PlayerStat.Strength);
}

int32 UABCharacterStatComponent::CalculateDefence(int32 CurrentLevel, int32 CurrentStrength)
{
	float LevelDefence = 0;
	float StrengthDefence = 0;

	int32 PrevThreshold = 0;

	for (const FDefenceSoftCap& Cap : DefenceSoftCaps)
	{
		int32 Range = FMath::Min(CurrentLevel, Cap.Threshold) - PrevThreshold;
		if (Range > 0)
		{
			// 해당하면 스탯 더하기
			LevelDefence += Range * Cap.Multiplier;
		}

		// Threshold 저장
		PrevThreshold = Cap.Threshold;

		// 현재 Threshold가 레벨 이상이면 for문 종료
		if (CurrentLevel <= Cap.Threshold) break;
	}
	
	StrengthDefence = ApplySoftCap(CurrentStrength, StrengthSoftCaps);

	return LevelDefence + StrengthDefence;
}

void UABCharacterStatComponent::LoadSoftCaps()
{
	// 데이터 테이블 확인
	if (!SoftCapTable) return;

	// 데이터 테이블에 있는 모든 데이터 가져오기
	static const FString Context = TEXT("SoftCap");
	TArray<FStatSoftCapRow*> Rows;
	SoftCapTable->GetAllRows(Context, Rows);
	TArray<FDefenceSoftCap*> DefRows;
	DefenceSoftCapTable->GetAllRows(Context, DefRows);

	// 소프트캡 등록
	for (FStatSoftCapRow* Row : Rows)
	{
		if (!Row) continue;
		FSoftCapData NewCap;
		NewCap.Threshold = Row->Threshold;
		NewCap.Multiplier = Row->Multiplier;

		switch (Row->StatType)
		{
		case EPlayerStatType::Vigor:
			VigorSoftCaps.Add(NewCap);
			break;

		case EPlayerStatType::Endurance:
			EnduranceSoftCaps.Add(NewCap);
			break;

		case EPlayerStatType::Strength:
			StrengthSoftCaps.Add(NewCap);
			break;
		}
	}

	for (FDefenceSoftCap* Row : DefRows)
	{
		if (!Row) return;

		FDefenceSoftCap NewCap;
		NewCap.Threshold = Row->Threshold;
		NewCap.Multiplier = Row->Multiplier;

		DefenceSoftCaps.Add(NewCap);
	}
}

float UABCharacterStatComponent::ApplySoftCap(int32 StatValue, const TArray<FSoftCapData>& SoftCaps)
{
	float Result = 0;
	int32 PrevThreshold = 0;

	for (const FSoftCapData& Cap : SoftCaps)
	{
		// 현재 레벨이 해당하는 구간에 있는지 확인
		int32 Range = FMath::Min(StatValue, Cap.Threshold) - PrevThreshold;
		if (Range > 0)
		{
			// 해당하면 스탯 더하기
			Result += Range * Cap.Multiplier;
		}

		// Threshold 저장
		PrevThreshold = Cap.Threshold;

		// 현재 Threshold가 레벨 이상이면 for문 종료
		if (StatValue <= Cap.Threshold) break;
	}

	return Result;
}

float UABCharacterStatComponent::GetMaxVigorHP()
{
	float TotalHP = 0.0f;
	int32 PrevThreshold = 0;

	for (const FSoftCapData& Cap : VigorSoftCaps)
	{
		int32 Threshold = FMath::Min(Cap.Threshold, 99);

		int32 Delta = Threshold - PrevThreshold;
		if (Delta > 0)
		{
			TotalHP += Delta * Cap.Multiplier;
			PrevThreshold = Threshold;
		}

		if (PrevThreshold >= 99) break;
	}

	return 400 + TotalHP;
}

float UABCharacterStatComponent::GetMaxEnduranceStamina()
{
	float TotalStamina = 0.0f;
	int32 PrevThreshold = 0;

	for (const FSoftCapData& Cap : EnduranceSoftCaps)
	{
		int32 Threshold = FMath::Min(Cap.Threshold, 99);

		int32 Delta = Threshold - PrevThreshold;
		if (Delta > 0)
		{
			TotalStamina += Delta * Cap.Multiplier;
			PrevThreshold = Threshold;
		}

		if (PrevThreshold >= 99) break;
	}

	return 100 + TotalStamina;
}

void UABCharacterStatComponent::ApplyStatData(const FPlayerStatData& NewStatData)
{
	PlayerStat = NewStatData;

	LoadSoftCaps();
	Level = PlayerStat.Vigor + PlayerStat.Endurance + PlayerStat.Strength;
	RecalculateDerivedStat();
	RecoverStamina();

	//OnSetHPStaminaSize.Broadcast(MaxHP, GetMaxVigorHP(), MaxStamina, GetMaxEnduranceStamina());
}
