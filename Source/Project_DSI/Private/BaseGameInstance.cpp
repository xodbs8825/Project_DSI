// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "InventoryManager.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

UBaseGameInstance::UBaseGameInstance()
{

}

void UBaseGameInstance::Init()
{
	Super::Init();

#if WITH_EDITOR
	FEditorDelegates::EndPIE.AddUObject(this, &UBaseGameInstance::OnPIEEnded);
#endif
}

void UBaseGameInstance::Shutdown()
{
	SaveGameData();

	Super::Shutdown();
}

#if WITH_EDITOR
void UBaseGameInstance::OnPIEEnded(bool bIsSimulating)
{
	SaveGameData();

	UE_LOG(LogTemp, Log, TEXT("Game Saved"));
}
#endif

void UBaseGameInstance::SaveGameData()
{
	if (!CurrentSave) return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveSlotName, 0)) return;

	UGameplayStatics::SaveGameToSlot(CurrentSave, CurrentSaveSlotName, 0);
	bIsDirty = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game saved successfully."));
	}
}

void UBaseGameInstance::LoadGameData(FString SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveSlotName = SlotName;

		CurrentSave = Cast<UDSI_SaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

		CurrentSave->bHasSaveNewlyCreated = false;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Save game loaded successfully."));
		}
	}
}

void UBaseGameInstance::CreateGameData()
{
	CurrentSave->VigorLevel = DefaultStat.Vigor;
	CurrentSave->EnduranceLevel = DefaultStat.Endurance;
	CurrentSave->StrengthLevel = DefaultStat.Strength;

	CurrentSave->Souls = 0;

	CurrentSave->LastBonfireID = NAME_None;

	CurrentSave->bHasValidPlayerTransform = false;

	CurrentSave->bHasSaveNewlyCreated = true;
}
