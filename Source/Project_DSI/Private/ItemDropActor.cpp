// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDropActor.h"
#include "InventoryManager.h"
#include "ABPlayerController.h"
#include "BaseCharacter.h"
#include "SoulsDropActor.h"
#include "InteractPromptWidget.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItemDropActor::AItemDropActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

	InteractPromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractPrompt"));
	InteractPromptComponent->SetupAttachment(Mesh);

	InteractPromptComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractPromptComponent->SetDrawAtDesiredSize(true);
	InteractPromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractPromptComponent->SetPivot(FVector2D(0.5f, 0.0f));

	InteractPromptComponent->SetRelativeScale3D(FVector(1.0f));
	InteractPromptComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));

	InteractPromptComponent->SetVisibility(false);

	// Physics 제거
	InteractPromptComponent->SetSimulatePhysics(false);
	InteractPromptComponent->SetEnableGravity(false);

	// 콜리전 제거
	InteractPromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 항상 카메라 보기
	InteractPromptComponent->SetTickWhenOffscreen(false);
	InteractPromptComponent->SetTranslucentSortPriority(10);

	InteractionText = FText::FromString(TEXT("Pick Up Item"));
}

void AItemDropActor::BeginPlay()
{
	Super::BeginPlay();

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		if (GI->CurrentSave->CollectedWorldItems.Contains(UniqueID) && DropType == EItemDropType::WorldItem)
		{
			Destroy();
		}
	}
}

void AItemDropActor::PickUp(UInventoryManager* Inventory)
{
	// 인벤토리에 아이템 추가
	Inventory->AddItem(ItemID, Count);

	UBaseGameInstance* GI = GetGameInstance<UBaseGameInstance>();
	if (GI && GI->CurrentSave)
	{
		if (DropType == EItemDropType::WorldItem)
		{
			GI->CurrentSave->CollectedWorldItems.AddUnique(UniqueID);
		}
		else
		{
			for (FDroppedItemData Data : GI->CurrentSave->DroppedItems)
			{
				if (Data.DropID == DropID)
				{
					GI->CurrentSave->DroppedItems.Remove(Data);
					break;
				}
			}
		}

		GI->MarkDirty();
	}

	Destroy();
}

void AItemDropActor::Interact_Implementation(APlayerController* InteractingController)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(InteractingController->GetPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	// 아이템 줍기
	PickUp(Inventory);
}

void AItemDropActor::SetInteractPromptVisible_Implementation(bool bVisible)
{
	if (!InteractPromptComponent) return;

	InteractPromptComponent->SetVisibility(bVisible);

	if (bVisible)
	{
		if (UUserWidget* Widget = InteractPromptComponent->GetUserWidgetObject())
		{
			if (UInteractPromptWidget* PromptWidget = Cast<UInteractPromptWidget>(Widget))
			{
				PromptWidget->SetPromptText(InteractionText);
			}
		}
	}
}
