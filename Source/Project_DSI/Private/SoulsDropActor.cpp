// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsDropActor.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"

// Sets default values
ASoulsDropActor::ASoulsDropActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASoulsDropActor::PickUp(UABCharacterStatComponent* StatComponent)
{
	StatComponent->PickUpSouls(SoulsAmount);
	Destroy();
}

void ASoulsDropActor::Interact_Implementation(APlayerController* InteractingController)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(InteractingController->GetPawn());
	if (!Player) return;

	UABCharacterStatComponent* StatComponent = Player->CharacterStatManager;
	if (!StatComponent) return;

	PickUp(StatComponent);

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->Souls += SoulsAmount;
		GI->CurrentSave->DroppedSoulsAmount = 0;

		GI->MarkDirty();
	}
}