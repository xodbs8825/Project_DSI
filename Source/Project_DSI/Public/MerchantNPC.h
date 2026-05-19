// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "BaseItem.h"
#include "MerchantNPC.generated.h"

class UUserWidget;

USTRUCT(BlueprintType)
struct FShopItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price;
};

UCLASS()
class PROJECT_DSI_API AMerchantNPC : public ANPCBase
{
	GENERATED_BODY()

public:
	AMerchantNPC();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<FShopItem> ShopItems;

public:
	virtual void Interact_Implementation(APlayerController* InteractingController) override;

	const TArray<FShopItem>& GetShopItems() const
	{
		return ShopItems;
	}

	const FItemData* GetItemData(FName ItemID) const;

private:
	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;
};
