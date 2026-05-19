// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "WeaponBaseItem.generated.h"

class AWeaponBaseActor;

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StrengthScaling = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AWeaponBaseActor> WeaponActor;
};

UCLASS()
class PROJECT_DSI_API AWeaponBaseItem : public ABaseItem
{
	GENERATED_BODY()

public:


	// 구조 변경
};
