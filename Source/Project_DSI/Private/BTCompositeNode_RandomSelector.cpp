// Fill out your copyright notice in the Description page of Project Settings.


#include "BTCompositeNode_RandomSelector.h"

UBTCompositeNode_RandomSelector::UBTCompositeNode_RandomSelector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), LastSuccessfulChildIndex(INDEX_NONE)
{
    NodeName = "Random Selector";
}

void UBTCompositeNode_RandomSelector::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
    InitializeNodeMemory<FBTCompositeMemory>(NodeMemory, InitType);
    ExecutedChildren.Empty();
    LastSuccessfulChildIndex = INDEX_NONE;
}

void UBTCompositeNode_RandomSelector::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
    CleanupNodeMemory<FBTCompositeMemory>(NodeMemory, CleanupType);
    ExecutedChildren.Empty();
    LastSuccessfulChildIndex = INDEX_NONE;
}

int32 UBTCompositeNode_RandomSelector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
    int32 NextChildIdx = BTSpecialChild::ReturnToParent;

    if (PrevChild == BTSpecialChild::NotInitialized)
    {
        // First execution, choose a random child
        NextChildIdx = FMath::RandRange(0, GetChildrenNum() - 1);
        if (NextChildIdx == LastSuccessfulChildIndex && GetChildrenNum() > 1)
        {
            // Avoid the last successful child being chosen first if possible
            NextChildIdx = (NextChildIdx + 1) % GetChildrenNum();
        }
    }
    else
    {
        // Handle last result
        if (LastResult == EBTNodeResult::Succeeded)
        {
            // If last child succeeded, return success and reset
            LastSuccessfulChildIndex = PrevChild;
            ExecutedChildren.Empty();
            return BTSpecialChild::ReturnToParent;
        }
        else if (LastResult == EBTNodeResult::Failed)
        {
            // If last child failed, mark it as executed
            ExecutedChildren.Add(PrevChild);

            // If all children have failed, return failure
            if (ExecutedChildren.Num() >= GetChildrenNum())
            {
                ExecutedChildren.Empty();
                LastSuccessfulChildIndex = INDEX_NONE;
                return BTSpecialChild::ReturnToParent;
            }

            // Choose a new random child that has not been executed yet
            TArray<int32> AvailableChildren;
            for (int32 ChildIdx = 0; ChildIdx < GetChildrenNum(); ++ChildIdx)
            {
                if (!ExecutedChildren.Contains(ChildIdx))
                {
                    AvailableChildren.Add(ChildIdx);
                }
            }

            // Choose a random child index from available children
            NextChildIdx = AvailableChildren[FMath::RandRange(0, AvailableChildren.Num() - 1)];
        }
    }

    return NextChildIdx;
}
