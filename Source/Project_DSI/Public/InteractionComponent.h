// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOntInteractTargetChanged, AActor*, NewTarget);

class APlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_DSI_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOntInteractTargetChanged OnTargetChanged;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ScanInterval = 0.1f;

	FTimerHandle ScanTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bUIBlocking = false;

protected:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float TraceDistance = 300.0f;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentInteractActor;

	UPROPERTY(BlueprintReadOnly)
	AActor* PerviousInteractActor;

public:	
	UFUNCTION(BlueprintCallable)
	void FindInteractable();

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	FText GetCurrentInteractText() const;

	UFUNCTION(BlueprintCallable)
	void SetUIBlocking(bool bBlock)
	{
		bUIBlocking = bBlock;
	}

	UFUNCTION(BlueprintCallable)
	void TryInteract();

protected:
	void FindNewInteractTarget();

	void ClearCurrentTarget();
};
