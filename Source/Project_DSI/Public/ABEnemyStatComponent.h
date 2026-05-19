// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ABEnemyStatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_DSI_API UABEnemyStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UABEnemyStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitEnemyData();

	FString GetEnemyName() { return EnemyName; }

	void SetCurrentHP(float NewHP) { CurrentHP = NewHP; }
	float GetCurrentHP() { return CurrentHP; }

	float GetMaxHP() { return MaxHP; }

	void SetCurrentPosture(float NewPosture) { CurrentPosture = NewPosture; }
	float GetCurrentPosture() { return CurrentPosture; }

	float GetMaxPosture() { return MaxPosture; }

	float GetDamage() { return Damage; }

	float GetDefense() { return Defense; }

private:
	UPROPERTY(EditAnywhere, Category = "Data")
	class UDataTable* EnemyStatTable;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	FString EnemyCode;

	struct FABEnemyData* CurrentEnemyData;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FString EnemyName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float MaxHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float CurrentPosture;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float MaxPosture;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Damage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Defense;
};