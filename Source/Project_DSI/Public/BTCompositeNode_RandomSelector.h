// Fill out your copyright notice in the Description page of Project Settings.
// https://dev.epicgames.com/community/snippets/Kyeo/unreal-engine-random-selector-behavior-tree-custom-composite-node?locale=ko-kr

#pragma once

#include "EngineMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTCompositeNode_RandomSelector.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API UBTCompositeNode_RandomSelector : public UBTCompositeNode
{
	GENERATED_BODY()
	
public:
	UBTCompositeNode_RandomSelector(const FObjectInitializer& ObjectInitializeer);

	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

private:
	mutable TArray<int32> ExecutedChildren;
	mutable int32 LastSuccessfulChildIndex;
};
