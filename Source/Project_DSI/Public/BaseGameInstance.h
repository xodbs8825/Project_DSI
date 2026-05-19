// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FDefaultStat
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 Vigor = 5;

	UPROPERTY(EditDefaultsOnly)
	int32 Endurance = 10;

	UPROPERTY(EditDefaultsOnly)
	int32 Strength = 5;
};

UCLASS()
class PROJECT_DSI_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UBaseGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

#if WITH_EDITOR
	// PIE 종료 감지용
	void OnPIEEnded(bool bIsSimulating);
#endif

	void SaveGameData();

	void LoadGameData(FString SlotName);
	void CreateGameData();

	void MarkDirty() { bIsDirty = true; }

private:
	bool bIsDirty = false;

public:
	UPROPERTY(EditDefaultsOnly)
	FDefaultStat DefaultStat;

	UPROPERTY(EditDefaultsOnly)
	float PlayerTransformUpdateInterval = 1.0f;

	UPROPERTY()
	class UDSI_SaveGame* CurrentSave;

	UPROPERTY()
	FString CurrentSaveSlotName;
};
