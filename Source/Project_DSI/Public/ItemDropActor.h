// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "ItemDropActor.generated.h"

UENUM(BlueprintType)
enum class EItemDropType : uint8
{
	WorldItem,
	EnemyDrop
};

UCLASS()
class PROJECT_DSI_API AItemDropActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemDropActor();

public:	
	virtual void BeginPlay() override;

	void PickUp(class UInventoryManager* Inventory);

	virtual void Interact_Implementation(APlayerController* InteractingController) override;
	virtual void SetInteractPromptVisible_Implementation(bool bVisible) override;

public:
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	class UWidgetComponent* InteractPromptComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText InteractionText;

	// 월드에 배치 된 아이템 용
	UPROPERTY(EditAnywhere)
	FName UniqueID;

	// 드랍 된 아이템 용
	UPROPERTY()
	FGuid DropID;

	UPROPERTY(EditAnywhere)
	FName ItemID;

	UPROPERTY(EditAnywhere)
	int32 Count;

	UPROPERTY(EditAnywhere)
	EItemDropType DropType = EItemDropType::EnemyDrop;
};
