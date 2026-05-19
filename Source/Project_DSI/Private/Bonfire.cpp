// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonfire.h"
#include "BaseCharacter.h"
#include "ABEnemyCharacter.h"
#include "ABAIController.h"
#include "BrainComponent.h"
#include "BonfireSubsystem.h"
#include "InteractInterface.h"
#include "ABPlayerController.h"
#include "ABCharacterStatComponent.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "Components/ArrowComponent.h"

// Sets default values
ABonfire::ABonfire()
{
	InteractionText = FText::FromString(TEXT("Rest at Bonfire"));
}

void ABonfire::Interact_Implementation(APlayerController* InteractingController)
{
	if (!InteractingController) return;

	AABPlayerController* PC = Cast<AABPlayerController>(InteractingController);
	if (PC)
	{
		// 화톳불 위젯 오픈
		PC->OpenBonfireUI(BonfireName);
	}

	// 쉬기
	Rest();

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	Player->CharacterStatManager->RecoverFull();

	if (UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>())
	{
		//GI->SaveGameData();

		if (GI->CurrentSave)
		{
			GI->CurrentSave->ActivatedBonfires.AddUnique(UniqueID);
			GI->CurrentSave->LastBonfireID = UniqueID;

			GI->SaveGameData();
		}
	}

}

void ABonfire::Rest()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBonfireSubsystem* Sub = GI->GetSubsystem<UBonfireSubsystem>())
		{
			// GameInstanceSubsystem을 부모로 가지고 있는 BonfireSubsystem에 등록되어 있는 모든 적 리셋
			Sub->ResetEnemies();
		}
	}
}

FTransform ABonfire::GetRespawnTransform() const
{
	FTransform Trans = GetArrowComponent()->GetComponentTransform();

	FVector Loc = Trans.GetLocation();
	Loc -= GetArrowComponent()->GetForwardVector() * 150;

	Trans.SetLocation(Loc);

	return Trans;
}
