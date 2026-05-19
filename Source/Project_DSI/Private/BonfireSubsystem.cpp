// Fill out your copyright notice in the Description page of Project Settings.


#include "BonfireSubsystem.h"
#include "ABEnemyCharacter.h"
#include "BaseCharacter.h"

void UBonfireSubsystem::RegisterEnemy(AABEnemyCharacter* Enemy)
{
	// 적 캐릭터 클래스에서 BeginPlay에서 등록
	Enemies.AddUnique(Enemy);
}

void UBonfireSubsystem::ResetEnemies()
{
	// 등록되어 있는 모든 적 리스폰
	for (auto Enemy : Enemies)
	{
		if (Enemy.IsValid())
		{
			Enemy->Respawn();
		}
	}
}
