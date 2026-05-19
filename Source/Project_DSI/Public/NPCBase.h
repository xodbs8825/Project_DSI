// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractInterface.h"
#include "Components/WidgetComponent.h"
#include "NPCBase.generated.h"

class USphereComponent;

UCLASS(Abstract)
class PROJECT_DSI_API ANPCBase : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPCBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	UWidgetComponent* InteractPromptComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText InteractionText;

public:	
	virtual void Interact_Implementation(APlayerController* InteractingController);

	virtual void SetInteractPromptVisible_Implementation(bool bVisible);

	virtual FText GetInteractText_Implementation() const;
};
