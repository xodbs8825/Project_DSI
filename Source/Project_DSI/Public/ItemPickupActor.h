// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "ItemPickupActor.generated.h"

UCLASS()
class PROJECT_DSI_API AItemPickupActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickupActor();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APlayerController* InteractingController);

protected:
	UPROPERTY(EditAnywhere)
	FName ItemID;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};
