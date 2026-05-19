// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SafeLocationFinder.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API USafeLocationFinder : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Safe Location")
	static bool FindSafeLocation(UObject* WorldContext, const FVector& OriginalLocation, const FVector& CapsuleExtent, FVector& OutSafeLocation);

private:
	static bool IsLocationSafe(UWorld* World, const FVector& Location, const FVector& CapsuleExtent);
};
