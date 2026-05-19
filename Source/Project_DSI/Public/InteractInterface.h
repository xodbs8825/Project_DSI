// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_DSI_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 상호작용
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(APlayerController* InteractingController);

	// 상호작용이 가능할 때 출력할 프롬프트 세팅
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetInteractPromptVisible(bool bVisible);

	// 프롬프트로 출력 될 텍스트 가져오기
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractText() const;
};
