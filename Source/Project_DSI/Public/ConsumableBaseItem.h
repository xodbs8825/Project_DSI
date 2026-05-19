// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ConsumableBaseItem.generated.h"

USTRUCT(BlueprintType)
struct FConsumableData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABaseItemEffect> ItemEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* UseMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool AllowMultiUse = false;
};

UCLASS()
class PROJECT_DSI_API AConsumableBaseItem : public ABaseItem
{
	GENERATED_BODY()
};
