// Fill out your copyright notice in the Description page of Project Settings.


#include "ABEnemyStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABEnemyStatComponent::UABEnemyStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UABEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UABEnemyStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UABEnemyStatComponent::InitEnemyData()
{
	if (EnemyStatTable != nullptr)
	{
		CurrentEnemyData = EnemyStatTable->FindRow<FABEnemyData>(*EnemyCode, TEXT(""));
		if (CurrentEnemyData != nullptr)
		{
			EnemyName = CurrentEnemyData->Name;
			CurrentHP = CurrentEnemyData->HP;
			MaxHP = CurrentEnemyData->HP;
			CurrentPosture = 0.0f;
			MaxPosture = CurrentEnemyData->MaxStamina;
			Damage = CurrentEnemyData->Damage;
			Defense = CurrentEnemyData->Defense;
		}
	}
}

