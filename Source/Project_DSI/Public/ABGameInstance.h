// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "ABGameInstance.generated.h"

/**
 *
 */

USTRUCT(BlueprintType)
struct FABEnemyData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABEnemyData() : Name(TEXT("Test")), Code(TEXT("001")), HP(100), Damage(10), Defense(5), MaxStamina(10) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Code;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 MaxStamina;
};


UCLASS()
class PROJECT_DSI_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UABGameInstance();

	virtual void Init() override;
};
