// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBaseActor.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AWeaponBaseActor::AWeaponBaseActor()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON_MESH"));
	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WEAPON_COLLISION"));
	BlockHitFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BLOCK_VFX"));

	WeaponMesh->SetStaticMesh(WeaponData.Mesh);
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetRelativeTransform(WeaponData.RelativeTransform);

	WeaponCollision->SetupAttachment(WeaponMesh);

	BlockHitFX->SetupAttachment(WeaponMesh);
}