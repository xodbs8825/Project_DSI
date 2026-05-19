// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickupActor.h"
#include "BaseCharacter.h"
#include "InventoryManager.h"

// Sets default values
AItemPickupActor::AItemPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickupActor::Interact_Implementation(APlayerController* InteractingController)
{
	if (!InteractingController) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(InteractingController->GetPawn());
	if (!Player) return;

	Player->InventoryManager->AddItem(ItemID, Count);

	Destroy();
}