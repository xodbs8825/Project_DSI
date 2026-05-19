// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DSI_CharacterIndexSave.generated.h"

USTRUCT(BlueprintType)
struct FCharacterSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime CreatedTime;
};

UCLASS()
class PROJECT_DSI_API UDSI_CharacterIndexSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<FCharacterSlotInfo> CharacterSlots;

	UPROPERTY()
	int32 LastPlayedID = -1;
};
