// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemPromptWidget.h"
#include "InventoryManager.h"
#include "ItemSlotWidget.h"
#include "InventoryWidget.h"
#include "InventoryDetailWidget.h"
#include "BaseItem.h"
#include "ConsumableBaseItem.h"
#include "BaseCharacter.h"
#include "BaseAnimInstance.h"
#include "ABPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UInventoryItemPromptWidget::NativeConstruct()
{
	UseButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnUseButtonClicked);
	DropButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnDropButtonClicked);
	DiscardButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnDiscardButtonClicked);
	DropAllButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnDropAllButtonClicked);
	DiscardAllButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnDiscardAllButtonClicked);
	CountUpButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnCountUpButtonClicked);
	CountDownButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnCountDownButtonClicked);
	YesButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnYesButtonClicked);
	NoButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnNoButtonClicked);
	ConfirmationYesButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnConfirmationYesButtonClicked);
	ConfirmationNoButton->OnReleased.AddDynamic(this, &UInventoryItemPromptWidget::OnConfirmationNoButtonClicked);
}

void UInventoryItemPromptWidget::SetPromptWidget(UItemSlotWidget* ItemSlot)
{
	if (!ItemSlot->IsInventoryItemValid() || !ItemSlot->IsItemDataValid()) return;

	Item = ItemSlot->GetInventoryItem();
	ItemData = ItemSlot->GetItemData();

	int32 ItemCount = Item->Count;
	EItemType ItemType = ItemData->ItemType;

	DropAllButton->SetIsEnabled(ItemCount >= 2 ? true : false);
	DiscardAllButton->SetIsEnabled(ItemCount >= 2 ? true : false);

	UseButton->SetIsEnabled(ItemType == EItemType::Consumable ? true : false);

	ResetPromptWidget(true);
}

void UInventoryItemPromptWidget::ResetPromptWidget(bool PromptWidgetVisibility)
{
	ItemOptionPanel->SetVisibility(ESlateVisibility::Visible);
	ItemCountPanel->SetVisibility(ESlateVisibility::Hidden);
	ConfirmPanel->SetVisibility(ESlateVisibility::Hidden);

	SetVisibility(PromptWidgetVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	ItemPromptState = EItemPromptState::None;

	CurrentItemCount = 1;
	ItemCountText->SetText(FText::AsNumber(CurrentItemCount));
}

void UInventoryItemPromptWidget::OnUseButtonClicked()
{
	if (!ItemData || !Item || !InventoryWidget) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	UBaseAnimInstance* Anim = Cast<UBaseAnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!Anim) return;

	const FConsumableData* Data = Inventory->GetConsumableData(Item->ItemID);
	if (!Data) return;

	if (!Data->UseMontage) return;

	MaxItemCount = Item->Count;
	if (MaxItemCount > 1 && Data->AllowMultiUse)
	{
		ItemPromptState = EItemPromptState::Use;

		ItemOptionPanel->SetVisibility(ESlateVisibility::Hidden);
		ItemCountPanel->SetVisibility(ESlateVisibility::Visible);

		return;
	}

	Anim->OnItemUsed.BindLambda([this]()
		{
			ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
			if (!Player) return;

			UInventoryManager* Inventory = Player->InventoryManager;
			if (!Inventory) return;

			Inventory->UseItem(Item->ItemID, 1);

			InventoryWidget->LoadInventory(Inventory, EItemType::Consumable);
		});

	Anim->Montage_Play(Data->UseMontage);

	//Inventory->UseItem(Item->ItemID, 1);
	InventoryWidget->LoadInventory(Inventory, ItemData->ItemType);
	ResetPromptWidget(false);

	ABPC->CloseInventoryUI();
}

void UInventoryItemPromptWidget::OnDropButtonClicked()
{
	if (!ItemData || !Item || !InventoryWidget) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	MaxItemCount = Item->Count;
	if (MaxItemCount > 1)
	{
		ItemPromptState = EItemPromptState::Drop;

		ItemOptionPanel->SetVisibility(ESlateVisibility::Hidden);
		ItemCountPanel->SetVisibility(ESlateVisibility::Visible);

		return;
	}

	Inventory->DropItem(Item->ItemID, 1);
	InventoryWidget->LoadInventory(Inventory, ItemData->ItemType);
	ResetPromptWidget(false);

	ABPC->CloseInventoryUI();
}

void UInventoryItemPromptWidget::OnDiscardButtonClicked()
{
	if (!Item) return;

	MaxItemCount = Item->Count;
	if (MaxItemCount > 1)
	{
		ItemPromptState = EItemPromptState::Discard;

		ItemOptionPanel->SetVisibility(ESlateVisibility::Hidden);
		ItemCountPanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventoryItemPromptWidget::OnDropAllButtonClicked()
{
	if (!ItemData || !Item) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	Inventory->DropItem(Item->ItemID, Item->Count);
	InventoryWidget->LoadInventory(Inventory, ItemData->ItemType);

	ABPC->CloseInventoryUI();
}

void UInventoryItemPromptWidget::OnDiscardAllButtonClicked()
{
	if (!Item) return;

	CurrentItemCount = Item->Count;

	ItemOptionPanel->SetVisibility(ESlateVisibility::Hidden);
	ConfirmPanel->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryItemPromptWidget::OnCountUpButtonClicked()
{
	if (MaxItemCount > CurrentItemCount)
	{
		CurrentItemCount++;
	}
	else
	{
		CurrentItemCount = 1;
	}

	ItemCountText->SetText(FText::AsNumber(CurrentItemCount));
}

void UInventoryItemPromptWidget::OnCountDownButtonClicked()
{
	if (CurrentItemCount > 1)
	{
		CurrentItemCount--;
	}
	else
	{
		CurrentItemCount = MaxItemCount;
	}

	ItemCountText->SetText(FText::AsNumber(CurrentItemCount));
}

void UInventoryItemPromptWidget::OnYesButtonClicked()
{
	if (!ItemData || !Item || !InventoryWidget) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	const FConsumableData* Data = Inventory->GetConsumableData(Item->ItemID);
	if (!Data) return;

	UBaseAnimInstance* Anim = Cast<UBaseAnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!Anim) return;

	switch (ItemPromptState)
	{
	case EItemPromptState::None:
		break;

	case EItemPromptState::Use:
		Anim->OnItemUsed.BindLambda([this]()
			{
				ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
				if (!Player) return;

				UInventoryManager* Inventory = Player->InventoryManager;
				if (!Inventory) return;

				Inventory->UseItem(Item->ItemID, CurrentItemCount);
			});

		Player->PlayAnimMontage(Data->UseMontage);

		//Inventory->UseItem(Item->ItemID, CurrentItemCount);

		break;

	case EItemPromptState::Drop:
		Inventory->DropItem(Item->ItemID, CurrentItemCount);

		break;

	case EItemPromptState::Discard:
		ItemCountPanel->SetVisibility(ESlateVisibility::Hidden);
		ConfirmPanel->SetVisibility(ESlateVisibility::Visible);

		return;

	default:
		break;
	}

	InventoryWidget->LoadInventory(Inventory, ItemData->ItemType);
	ResetPromptWidget(false);
	InventoryWidget->FocusState = EItemSlotFocusState::None;
	InventoryWidget->DetailWidget->SetVisibility(ESlateVisibility::Hidden);

	ABPC->CloseInventoryUI();
}

void UInventoryItemPromptWidget::OnNoButtonClicked()
{
	ResetPromptWidget(true);
}

void UInventoryItemPromptWidget::OnConfirmationYesButtonClicked()
{
	if (!ItemData || !Item || !InventoryWidget) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AABPlayerController* ABPC = Cast<AABPlayerController>(PC);
	if (!ABPC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	Inventory->RemoveItem(Item->ItemID, CurrentItemCount);

	InventoryWidget->LoadInventory(Inventory, ItemData->ItemType);
	ResetPromptWidget(false);
	InventoryWidget->FocusState = EItemSlotFocusState::None;
	InventoryWidget->DetailWidget->SetVisibility(ESlateVisibility::Hidden);

	ABPC->CloseInventoryUI();
}

void UInventoryItemPromptWidget::OnConfirmationNoButtonClicked()
{
	ResetPromptWidget(true);
}
