// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "SoulsDropActor.generated.h"

UCLASS()
class PROJECT_DSI_API ASoulsDropActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASoulsDropActor();

public:
	void PickUp(class UABCharacterStatComponent* StatComponent);

	virtual void Interact_Implementation(APlayerController* InteractingController) override;

public:
	UPROPERTY(EditDefaultsOnly)
	int32 SoulsAmount;

};
