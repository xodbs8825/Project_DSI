// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "InteractionComponent.h"
#include "InteractInterface.h"
#include "InteractPromptWidget.h"
#include "NPCBase.h"
#include "MerchantNPC.h"
#include "ShopWidget.h"
#include "BaseCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Bonfire.h"
#include "BonfireWidget.h"
#include "LevelUpWidget.h"
#include "InventoryWidget.h"
#include "EquipmentViewWidget.h"
#include "EquipItemWidget.h"
#include "InventoryManager.h"
#include "InventoryItemPromptWidget.h"
#include "DeathWidget.h"
#include "BonfireWidget.h"
#include "FadeWidget.h"

AABPlayerController::AABPlayerController()
{
}

void AABPlayerController::OpenInventoryUI(UInventoryManager* Inventory)
{
	if (!Inventory || !InventoryWidgetClass) return;

	if (!InventoryWidget)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		InventoryWidget->AddToViewport(10);
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(InventoryWidget);
	}

	if (IsOtherWidgetOpen(InventoryWidget)) return;

	InventoryWidget->SetInventoryItems(Inventory->Inventory);
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->LoadInventory(Inventory, EItemType::Consumable);

	SetUIInputMode(InventoryWidget, true);
}

void AABPlayerController::CloseInventoryUI()
{
	if (!InventoryWidget) return;

	InventoryWidget->InitInventoryWidget();
	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	SetUIInputMode(InventoryWidget, false);
}

void AABPlayerController::OpenShopUI(AMerchantNPC* MerchantNPC)
{
	if (!MerchantNPC || !ShopWidgetClass) return;

	if (!ShopWidget)
	{
		ShopWidget = CreateWidget<UShopWidget>(this, ShopWidgetClass);
		ShopWidget->AddToViewport(10);
		ShopWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(ShopWidget);
	}

	if (IsOtherWidgetOpen(ShopWidget)) return;

	ShopWidget->SetVisibility(ESlateVisibility::Visible);
	ShopWidget->SetShopItems(MerchantNPC->GetShopItems());

	SetUIInputMode(ShopWidget, true);
}

void AABPlayerController::CloseShopUI()
{
	if (!ShopWidget) return;

	ShopWidget->SetVisibility(ESlateVisibility::Hidden);

	SetUIInputMode(ShopWidget, false);
}

void AABPlayerController::OpenBonfireUI(FName BonfireName)
{
	if (!BonfireWidgetClass) return;

	if (!BonfireWidget)
	{
		BonfireWidget = CreateWidget<UBonfireWidget>(this, BonfireWidgetClass);
		BonfireWidget->AddToViewport(10);
		BonfireWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(BonfireWidget);
	}

	if (IsOtherWidgetOpen(BonfireWidget)) return;

	BonfireWidget->SetBonfireName(BonfireName);
	BonfireWidget->SetVisibility(ESlateVisibility::Visible);
	CurrentBonfireName = BonfireName;

	SetUIInputMode(BonfireWidget, true);
}

void AABPlayerController::CloseBonfireUI()
{
	if (!BonfireWidget) return;

	BonfireWidget->SetVisibility(ESlateVisibility::Hidden);

	SetUIInputMode(BonfireWidget, false);
}

void AABPlayerController::OpenLevelUpUI()
{
	if (!LevelUpWidgetClass) return;
	if (!BonfireWidget) return;

	BonfireWidget->SetVisibility(ESlateVisibility::Hidden);

	if (!LevelUpWidget)
	{
		LevelUpWidget = CreateWidget<ULevelUpWidget>(this, LevelUpWidgetClass);
		LevelUpWidget->AddToViewport(10);
		LevelUpWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(LevelUpWidget);
	}

	if (IsOtherWidgetOpen(LevelUpWidget)) return;

	LevelUpWidget->SetVisibility(ESlateVisibility::Visible);
	LevelUpWidget->LoadPlayerStats();

	SetUIInputMode(LevelUpWidget, true);
}

void AABPlayerController::CloseLevelUpUI()
{
	if (!LevelUpWidget) return;

	LevelUpWidget->SetVisibility(ESlateVisibility::Hidden);

	OpenBonfireUI(CurrentBonfireName);
}

void AABPlayerController::OpenEquipmentUI(UInventoryManager* Inventory)
{
	if (!EquipmentWidgetClass) return;

	if (!EquipmentWidget)
	{
		EquipmentWidget = CreateWidget<UEquipmentViewWidget>(this, EquipmentWidgetClass);
		EquipmentWidget->AddToViewport(10);
		EquipmentWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(EquipmentWidget);
	}

	if (IsOtherWidgetOpen(EquipmentWidget)) return;

	EquipmentWidget->LoadEquipment(Inventory);
	EquipmentWidget->SetVisibility(ESlateVisibility::Visible);

	SetUIInputMode(EquipmentWidget, true);
}

void AABPlayerController::CloseEquipmentUI()
{
	if (!EquipmentWidget) return;

	EquipmentWidget->SetVisibility(ESlateVisibility::Hidden);

	if (EquipItemWidget)
	{
		EquipItemWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	SetUIInputMode(EquipmentWidget, false);
}

void AABPlayerController::OpenEquipItemUI(UInventoryManager* Inventory, EEquipmentSlotType InSlotType)
{
	if (!EquipItemWidgetClass) return;
	if (!EquipmentWidget) return;

	EquipmentWidget->SetVisibility(ESlateVisibility::Hidden);

	if (!EquipItemWidget)
	{
		EquipItemWidget = CreateWidget<UEquipItemWidget>(this, EquipItemWidgetClass);
		EquipItemWidget->AddToViewport(10);
		EquipItemWidget->SetVisibility(ESlateVisibility::Hidden);
		Widgets.Add(EquipItemWidget);
	}

	if (IsOtherWidgetOpen(EquipItemWidget)) return;

	EquipItemWidget->OnTabSelected(InSlotType);
	EquipItemWidget->SetVisibility(ESlateVisibility::Visible);

	SetUIInputMode(EquipItemWidget, true);
}

void AABPlayerController::CloseEquipItemUI(UInventoryManager* Inventory)
{
	if (!EquipItemWidget) return;

	EquipItemWidget->SetVisibility(ESlateVisibility::Hidden);

	OpenEquipmentUI(Inventory);
}

void AABPlayerController::CloseAllWidget()
{
	CloseInventoryUI();
	CloseShopUI();
	CloseBonfireUI();
	CloseLevelUpUI();
	CloseEquipmentUI();
}

bool AABPlayerController::IsInventoryUIOpen() const
{
	if (!IsInventoryUIValid()) return false;

	return InventoryWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsShopUIOpen() const
{
	if (!IsShopUIValid()) return false;

	return ShopWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsBonfireUIOpen() const
{
	if (!IsBonfireUIValid()) return false;

	return BonfireWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsLevelUpUIOpen() const
{
	if (!IsLevelUpUIValid()) return false;

	return LevelUpWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsEquipmentUIOpen() const
{
	if (!IsEquipmentUIValid()) return false;

	return EquipmentWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsEquipItemUIOpen() const
{
	if (!IsEquipItemUIValid()) return false;

	return EquipItemWidget->GetVisibility() == ESlateVisibility::Visible;
}

bool AABPlayerController::IsOtherWidgetOpen(UUserWidget* Widget) const
{
	if (Widgets.IsEmpty()) return false;
	if (!Widget) return false;

	for (auto& UW : Widgets)
	{
		if (UW != Widget)
		{
			if (UW->GetVisibility() == ESlateVisibility::Visible)
			{
				return true;
			}
		}
	}

	return false;
}

void AABPlayerController::CreateDeathWidget()
{
	if (!DeathWidgetClass) return;

	DeathWidget = CreateWidget<UDeathWidget>(this, DeathWidgetClass);
	if (!DeathWidget) return;

	DeathWidget->AddToViewport();
}

void AABPlayerController::TriggerDeathWidget()
{
	if (!DeathWidget) return;
	DeathWidget->OnDeathTriggered.Broadcast();
}

void AABPlayerController::SetInputMappingContext(UInputMappingContext* IMC)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(IMC, 0);

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("IMC changed to %s"), *IMC->GetName()));
	}
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (FadeWidgetClass)
	{
		UFadeWidget* FadeWidget = CreateWidget<UFadeWidget>(this, FadeWidgetClass);
		if (FadeWidget)
		{
			FadeWidget->AddToViewport(100);
			FadeWidget->OnFadeIn.Broadcast();
		}
	}
}

void AABPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	InteractionComponent = ControlledPawn->FindComponentByClass<UInteractionComponent>();
	if (InteractionComponent)
	{
		InteractionComponent->OnTargetChanged.AddDynamic(this, &AABPlayerController::OnInteractTargetChanged);
	}
}

void AABPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AABPlayerController::OnInteractTargetChanged(AActor* NewTarget)
{
	// 이전 액터의 상호작용 프롬프트 안보이게 변경
	if (PreviousTarget && PreviousTarget->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		IInteractInterface::Execute_SetInteractPromptVisible(PreviousTarget, false);
	}

	// 새로운 액터의 상호작용 프롬프트 보이게 변경
	if (NewTarget && NewTarget->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		IInteractInterface::Execute_SetInteractPromptVisible(NewTarget, true);
	}

	// 액터 업데이트
	PreviousTarget = NewTarget;
}

void AABPlayerController::SetUIInputMode(UUserWidget* Widget, bool IsUIOpened)
{
	if (IsUIOpened)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(Widget->TakeWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;

		OnUIOpened.Broadcast(this);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;

		OnUIClosed.Broadcast(this);
	}
}
