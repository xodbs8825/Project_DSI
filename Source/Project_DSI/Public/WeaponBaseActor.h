// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseActor.generated.h"

USTRUCT(BlueprintType)
struct FWeaponMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	FTransform RelativeTransform;
};

UCLASS()
class PROJECT_DSI_API AWeaponBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBaseActor();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	class UBoxComponent* WeaponCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* BlockHitFX;

private:
	UPROPERTY(EditAnywhere)
	FWeaponMeshData WeaponData;
};
