// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBase.h"
#include "GameFramework/Actor.h"
#include "Bonfire.generated.h"

UCLASS()
class PROJECT_DSI_API ABonfire : public ANPCBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABonfire();

public:
	virtual void Interact_Implementation(APlayerController* InteractingController) override;

	void Rest();

	FTransform GetRespawnTransform() const;

public:
	UPROPERTY(EditAnywhere)
	FName UniqueID;

	UPROPERTY(EditAnywhere)
	FName BonfireName;
};
