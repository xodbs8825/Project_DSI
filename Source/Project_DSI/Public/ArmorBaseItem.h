// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ArmorBaseItem.generated.h"

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	HeadArmor	UMETA(DisplayName = "Helms"),
	BodyArmor	UMETA(DisplayName = "Body Armor"),
	GloveArmor	UMETA(DisplayName = "Gauntlets"),
	LegArmor	UMETA(DisplayName = "Pants"),
	FootArmor	UMETA(DisplayName = "Boots")
};

USTRUCT(BlueprintType)
struct FArmorData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDefence = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EArmorType ArmorType;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//TSubclassOf<AArmorBaseActor> WeaponActor;
};

UCLASS()
class PROJECT_DSI_API AArmorBaseItem : public ABaseItem
{
	GENERATED_BODY()
	
};
