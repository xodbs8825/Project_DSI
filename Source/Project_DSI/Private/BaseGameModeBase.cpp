// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameModeBase.h"
#include "BaseCharacter.h"
#include "BasePlayerController.h"
#include "ABPlayerState.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "ItemDropActor.h"
#include "SoulsDropActor.h"

ABaseGameModeBase::ABaseGameModeBase()
{
	DefaultPawnClass = ABaseCharacter::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
}

void ABaseGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	SpawnDroppedItems();
	SpawnDroppedSouls();
}

void ABaseGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

void ABaseGameModeBase::SpawnDroppedItems()
{
	if (!ItemDropClass) return;

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (!GI || !GI->CurrentSave) return;

	for (FDroppedItemData Data : GI->CurrentSave->DroppedItems)
	{
		AItemDropActor* Drop = GetWorld()->SpawnActor<AItemDropActor>(ItemDropClass, Data.Transform);
		if (!Drop) continue;

		Drop->ItemID = Data.ItemID;
		Drop->Count = Data.Count;
		Drop->DropID = Data.DropID;
	}
}

void ABaseGameModeBase::SpawnDroppedSouls()
{
	if (!SoulsDropClass) return;

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (!GI || !GI->CurrentSave) return;

	if (GI->CurrentSave->DroppedSoulsAmount > 0)
	{
		ASoulsDropActor* Souls = GetWorld()->SpawnActor<ASoulsDropActor>(SoulsDropClass, GI->CurrentSave->DroppedSoulsTransform);
		if (!Souls) return;

		Souls->SoulsAmount = GI->CurrentSave->DroppedSoulsAmount;
	}
}
