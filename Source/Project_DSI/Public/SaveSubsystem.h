// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DSI_CharacterIndexSave.h"
#include "DSI_SaveGame.h"
#include "SaveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadIndex();
	void SaveIndex();

	int32 GenerateCharacterID() const;

public:
	bool CreateCharacter(const FString& CharacterName);
	UDSI_SaveGame* LoadCharacter(int32 CharacterID);
	UDSI_SaveGame* ContinueGame();

	const TArray<FCharacterSlotInfo>& GetCharacterSlots() const { return IndexSave->CharacterSlots; }
	bool DoesCharacterSlotsExist();

private:
	UPROPERTY()
	UDSI_CharacterIndexSave* IndexSave;

	FString IndexSlotName = TEXT("CharacterIndex");
};
