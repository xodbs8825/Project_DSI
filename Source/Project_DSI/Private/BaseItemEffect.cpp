// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItemEffect.h"
#include "BaseCharacter.h"

// Sets default values
ABaseItemEffect::ABaseItemEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void ABaseItemEffect::BeginPlay()
{
	Super::BeginPlay();

}
