// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable		UMETA(DisplayName = "Consumable"),
	Weapon			UMETA(DisplayName = "Weapon"),
	HeadArmor		UMETA(DisplayName = "Helms"),
	BodyArmor		UMETA(DisplayName = "Body Armor"),
	GloveArmor		UMETA(DisplayName = "Gauntlets"),
	LegArmor		UMETA(DisplayName = "Pants"),
	FootArmor		UMETA(DisplayName = "Boots"),
	KeyItem			UMETA(DisplayName = "Key Items"),
	UpgradeMaterial UMETA(DisplayName = "Materials")
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TSubclassOf<ABaseItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 ItemMaxStack;
};

UCLASS()
class PROJECT_DSI_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:
	ABaseItem();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ItemDataTable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class ABaseCharacter* OwningPlayerCharacter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FItemData ItemData;
};
