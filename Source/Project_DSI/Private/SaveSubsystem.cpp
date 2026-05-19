// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystem.h"
#include "DSI_SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "BaseGameInstance.h"

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadIndex();
}

void USaveSubsystem::LoadIndex()
{
	if (UGameplayStatics::DoesSaveGameExist(IndexSlotName, 0))
	{
		IndexSave = Cast<UDSI_CharacterIndexSave>(UGameplayStatics::LoadGameFromSlot(IndexSlotName, 0));
	}
	else
	{
		IndexSave = Cast<UDSI_CharacterIndexSave>(UGameplayStatics::CreateSaveGameObject(UDSI_CharacterIndexSave::StaticClass()));

		SaveIndex();
	}
}

void USaveSubsystem::SaveIndex()
{
	UGameplayStatics::SaveGameToSlot(IndexSave, IndexSlotName, 0);
}

int32 USaveSubsystem::GenerateCharacterID() const
{
	int32 MaxID = 0;

	for (const auto& Slot : IndexSave->CharacterSlots)
	{
		MaxID = FMath::Max(MaxID, Slot.CharacterID);
	}

	return MaxID + 1;
}

bool USaveSubsystem::CreateCharacter(const FString& CharacterName)
{
	const int32 NewID = GenerateCharacterID();
	const FString SlotName = FString::Printf(TEXT("Character_%d"), NewID);

	// 세이브 생성
	UDSI_SaveGame* CharacterSave = Cast<UDSI_SaveGame>(UGameplayStatics::CreateSaveGameObject(UDSI_SaveGame::StaticClass()));
	CharacterSave->CharacterID = NewID;
	CharacterSave->CharacterName = CharacterName;

	UBaseGameInstance* GI = GetWorld()->GetGameInstance<UBaseGameInstance>();
	if (GI)
	{
		GI->CurrentSave = CharacterSave;
		GI->CurrentSaveSlotName = SlotName;
		GI->CreateGameData();
	}

	UGameplayStatics::SaveGameToSlot(CharacterSave, SlotName, 0);

	// 인덱스 등록
	FCharacterSlotInfo NewSlot;
	NewSlot.CharacterID = NewID;
	NewSlot.CharacterName = CharacterName;
	NewSlot.SlotName = SlotName;

	IndexSave->CharacterSlots.Add(NewSlot);
	IndexSave->LastPlayedID = NewID;

	SaveIndex();

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Creating Player ID : %d"), IndexSave->LastPlayedID));

	return true;
}

UDSI_SaveGame* USaveSubsystem::LoadCharacter(int32 CharacterID)
{
	for (auto& Slot : IndexSave->CharacterSlots)
	{
		if (Slot.CharacterID == CharacterID)
		{
			IndexSave->LastPlayedID = CharacterID;

			SaveIndex();

			UBaseGameInstance* GI = GetWorld()->GetGameInstance<UBaseGameInstance>();
			if (GI)
			{
				GI->LoadGameData(Slot.SlotName);
			}

			return Cast<UDSI_SaveGame>(UGameplayStatics::LoadGameFromSlot(Slot.SlotName, 0));
		}
	}

	return nullptr;
}

UDSI_SaveGame* USaveSubsystem::ContinueGame()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Loading Player ID : %d"), IndexSave->LastPlayedID));

	return LoadCharacter(IndexSave->LastPlayedID);
}

bool USaveSubsystem::DoesCharacterSlotsExist()
{
	for (auto& Slot : GetCharacterSlots())
	{
		if (UGameplayStatics::DoesSaveGameExist(Slot.SlotName, 0))
		{
			return true;
		}
	}

	return false;
}
