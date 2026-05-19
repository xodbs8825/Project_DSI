// Fill out your copyright notice in the Description page of Project Settings.


#include "SafeLocationFinder.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"

bool USafeLocationFinder::FindSafeLocation(UObject* WorldContext, const FVector& OriginalLocation, const FVector& CapsuleExtent, FVector& OutSafeLocation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;

	// 1. 원래 위치가 안전하면 그대로 반환
	if (IsLocationSafe(World, OriginalLocation, CapsuleExtent))
	{
		OutSafeLocation = OriginalLocation;

		return true;
	}

	// 2. 주변 위치를 나선형으로 탐색
	// 탐색 반경을 점점 넓히면서 탐색
	const float StepSize = 50.0f;	// 탐색 간격
	const int32 MaxSteps = 10;		// 최대 탐색 반경 (50 * 10 = 500cm)
	const int32 AngleSteps = 8;		// 방향 개수 (360 / 8 = 45도 간격)

	for (int32 Step = 1; Step <= MaxSteps; Step++)
	{
		float Radius = StepSize * Step;

		for (int32 AngleIndex = 0; AngleIndex < AngleSteps; AngleIndex++)
		{
			float Angle = (360.0f / AngleSteps) * AngleIndex;
			float Rad = FMath::DegreesToRadians(Angle);

			FVector CandidateLocation = OriginalLocation + FVector(Radius * FMath::Cos(Rad), Radius * FMath::Sin(Rad), 0);

			// 바닥 높이 맞추기
			FHitResult HitResult;
			FVector TraceStart = CandidateLocation + FVector(0, 0, 500.0f);
			FVector TraceEnd = CandidateLocation - FVector(0, 0, 500.0f);

			if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic))
			{
				CandidateLocation.Z = HitResult.Location.Z + CapsuleExtent.Z;
			}

			if (IsLocationSafe(World, CandidateLocation, CapsuleExtent))
			{
				UE_LOG(LogTemp, Log, TEXT("Safe Location Found : %s (Step: %d, Angle: %f)"), *OutSafeLocation.ToString(), Step, Angle);

				OutSafeLocation = CandidateLocation;
				return true;
			}
		}
	}

	// 3. 못 찾으면 원래 위치 반환
	UE_LOG(LogTemp, Warning, TEXT("Safe Location Not Found! Using Original Location."));

	OutSafeLocation = OriginalLocation;
	return false;
}

bool USafeLocationFinder::IsLocationSafe(UWorld* World, const FVector& Location, const FVector& CapsuleExtent)
{
	// Block 콜리전과 겹치는지 체크
	TArray<FOverlapResult> Overlaps;

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleExtent.X, CapsuleExtent.Z);

	FCollisionQueryParams QueryParams;
	QueryParams.bFindInitialOverlaps = true;

	// ECC_Pawn 채널로 Block 콜리전만 체크
	bool bBlocked = World->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity, ECC_Pawn, CapsuleShape, QueryParams);
	if (!bBlocked) return true;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (Overlap.Component.IsValid())
		{
			ECollisionResponse Response = Overlap.Component->GetCollisionResponseToChannel(ECC_Pawn);
			if (Response == ECollisionResponse::ECR_Block)
			{
				return false;	// Block 콜리전과 겹침
			}
		}
	}

	return true;
}
