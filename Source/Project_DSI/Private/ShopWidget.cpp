// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopWidget.h"
#include "MerchantNPC.h"
#include "ShopItemSlotWidget.h"
#include "ShopItemDetailWidget.h"
#include "Components/PanelWidget.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "InventoryManager.h"
#include "ItemTypeTabWidget.h"
#include "BaseItem.h"
#include "ConsumableBaseItem.h"

// 상점 위젯을 열때 호출
void UShopWidget::SetShopItems(const TArray<FShopItem>& InItems)
{
	CurrentShopItems = InItems;

	// 아이템 탭 패널 초기화
	ItemTypesPanel->ClearChildren();

	if (!ItemListPanel || !ItmeSlotClass) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* PlayerInventory = Cast<UInventoryManager>(Player->InventoryManager);
	if (!PlayerInventory) return;

	TArray<EItemType> ItemTypes;

	// 현재 상인 NPC가 가지고 있는 아이템 타입 확인
	for (const FShopItem& Item : InItems)
	{
		// 아이템 데이터 데이터 테이블에서 로드
		const FItemData* Data = PlayerInventory->GetItemData(Item.ItemID);
		if (!Data) continue;

		// 아이템 타입 중복 확인
		if (!ItemTypes.Contains(Data->ItemType))
		{
			// 중복되지 않으면 추가
			ItemTypes.Add(Data->ItemType);
		}
	}

	// 순서대로 정렬
	ItemTypes.Sort();

	if (!ItemTypes.IsEmpty())
	{
		// 무조건 상점 위젯을 열때는 가장 첫 번째 타입의 아이템(enum에서 가장 낮은 값을 가지는 타입)
		CurrentItemType = ItemTypes[0];

		for (EItemType Type : ItemTypes)
		{
			// 아이템 탭 생성
			UItemTypeTabWidget* Tab = CreateWidget<UItemTypeTabWidget>(this, ItemTypeTabClass);
			if (!Tab) continue;

			Tab->SetItemType(Type);
			Tab->SetIsFocused(CurrentItemType == Type ? true : false);

			Tab->OnItemTypeChanged.AddDynamic(this, &UShopWidget::ItemTypeChanged);

			ItemTypesPanel->AddChild(Tab);

			ItemTypeMap.Add(Type, Tab);
		}
	}

	LoadShopItems(InItems, CurrentItemType, PlayerInventory);
}

bool UShopWidget::IsSlotSelected(const UShopItemSlotWidget* ItemSlot) const
{
	return ItemSlot && ItemSlot == SelectedSlot;
}

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(DetailWidget);

	DetailWidget->Hide();

	DetailWidget->OnPurchaseQuantityUp.AddDynamic(this, &UShopWidget::PurchaseCountUp);
	DetailWidget->OnPurchaseQuantityDown.AddDynamic(this, &UShopWidget::PurchaseCountDown);
}

void UShopWidget::HandleHovered(UShopItemSlotWidget* HoveredSlot)
{
	if (FocusState == EShopItemFocusState::None)
	{
		FocusedItem = HoveredSlot->GetShopItem();
		FocusState = EShopItemFocusState::Hovered;

		FItemData ItemData = HoveredSlot->GetItemData();

		DetailWidget->ShowItem(*FocusedItem, ItemData);
		DetailWidget->HidePurchaseCount();
		DetailWidget->SetPurchaseCountText(1);

		EItemType IT = ItemData.ItemType;
		bool bValue = (IT == EItemType::Consumable || IT == EItemType::UpgradeMaterial);

		DetailWidget->SetItemQuantityVisibility(bValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UShopWidget::HandleUnhovered()
{
	if (FocusState == EShopItemFocusState::Hovered)
	{
		FocusedItem = nullptr;
		FocusState = EShopItemFocusState::None;

		DetailWidget->Hide();
		DetailWidget->HidePurchaseCount();
		DetailWidget->SetPurchaseCountText(1);
	}
}

void UShopWidget::HandleClicked(UShopItemSlotWidget* ClickedSlot)
{
	SelectedSlot = ClickedSlot;
	FocusedItem = ClickedSlot->GetShopItem();
	FocusState = EShopItemFocusState::Selected;

	FItemData ItemData = ClickedSlot->GetItemData();

	DetailWidget->ShowItem(*FocusedItem, ItemData);

	if (ItemData.ItemType == EItemType::Consumable || ItemData.ItemType == EItemType::UpgradeMaterial)
	{
		DetailWidget->ShowPurchaseCount();
		DetailWidget->SetPurchaseCountText(1);
		if (GetMaxPurchaseCount() <= 1)
		{
			DetailWidget->EnablePurchaseCounter(false);
		}
		else
		{
			DetailWidget->EnablePurchaseCounter(true);
		}
	}
	else
	{
		DetailWidget->HidePurchaseCount();
		DetailWidget->SetPurchaseCountText(1);
	}
}

void UShopWidget::HandleHoldProgress(float Ratio, UShopItemSlotWidget* PressedSlot)
{
	if (!IsSlotSelected(PressedSlot)) return;
	//if (FocusState != EShopItemFocusState::Selected) return;
	if (!DetailWidget) return;
	if (!PressedSlot) return;

	PressedSlot->UpdateHoldProgressBar(FMath::Clamp(Ratio, 0.0f, 1.0f));
}

void UShopWidget::HandleHoldCompleted(UShopItemSlotWidget* ItemSlot)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* PlayerInventory = Player->InventoryManager;
	if (!PlayerInventory) return;

	if (FocusState == EShopItemFocusState::Selected && FocusedItem)
	{
		PurchaseItem(*FocusedItem);
		ItemSlot->CancelHold();
	}
}

void UShopWidget::PurchaseCountUp()
{
	if (CurrentPurchaseCount >= GetMaxPurchaseCount())
	{
		CurrentPurchaseCount = 1;
	}
	else
	{
		CurrentPurchaseCount++;
	}

	DetailWidget->SetPurchaseCountText(CurrentPurchaseCount);
}

void UShopWidget::PurchaseCountDown()
{
	if (CurrentPurchaseCount <= 1)
	{
		if (GetMaxPurchaseCount() <= 1)
		{
			CurrentPurchaseCount = 1;
		}
		else
		{
			CurrentPurchaseCount = GetMaxPurchaseCount();
		}
	}
	else
	{
		CurrentPurchaseCount--;
	}

	DetailWidget->SetPurchaseCountText(CurrentPurchaseCount);
}

void UShopWidget::ItemTypeChanged(EItemType NewType)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* PlayerInventory = Cast<UInventoryManager>(Player->InventoryManager);
	if (!PlayerInventory) return;

	LoadShopItems(CurrentShopItems, NewType, PlayerInventory);

	for (auto& Elem : ItemTypeMap)
	{
		if (NewType != Elem.Key)
		{
			Elem.Value->SetIsFocused(false);
		}
	}

	FocusState = EShopItemFocusState::None;
	if (!DetailWidget) return;

	DetailWidget->Hide();
}

void UShopWidget::PurchaseItem(const FShopItem& Item)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* Player = Cast<ABaseCharacter>(PC->GetPawn());
	if (!Player) return;

	UABCharacterStatComponent* PlayerStat = Cast<UABCharacterStatComponent>(Player->CharacterStatManager);
	if (!PlayerStat) return;

	UInventoryManager* PlayerInventory = Cast<UInventoryManager>(Player->InventoryManager);
	if (!PlayerInventory) return;

	if (PlayerStat->GetSouls() < Item.Price * CurrentPurchaseCount)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Failed to purchase item"));
		return;
	}

	PlayerStat->AddSouls(-Item.Price * CurrentPurchaseCount);

	PlayerInventory->AddItem(Item.ItemID, CurrentPurchaseCount);


	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Current Gold : %i"), PlayerStat->GetSouls()));

	FocusState = EShopItemFocusState::None;
	FocusedItem = nullptr;
	CurrentPurchaseCount = 1;
	DetailWidget->Hide();
	DetailWidget->SetPurchaseCountText(1);
}

void UShopWidget::LoadShopItems(const TArray<FShopItem>& InItems, EItemType InType, class UInventoryManager* Inventory)
{
	if (!Inventory) return;

	// 아이템 패널 초기화
	ItemListPanel->ClearChildren();

	TArray<FItemData> TypeMatchedItems;
	TArray<FShopItem> TypeMatchedShopItems;
	for (const FShopItem& Item : InItems)
	{
		const FItemData* Data = Inventory->GetItemData(Item.ItemID);
		if (!Data) continue;

		if (Data->ItemType == InType)
		{
			TypeMatchedItems.Add(*Data);
			TypeMatchedShopItems.Add(Item);
		}
	}

	int32 ItemCount = TypeMatchedItems.Num();
	int32 RowCount = FMath::Max(CurrentRowCount, FMath::CeilToInt((float)ItemCount / ColumnCount));
	int32 TotalSlotCount = RowCount * ColumnCount;

	for (int32 i = 0; i < TotalSlotCount; i++)
	{
		// 아이템 슬롯 위젯 생성
		UShopItemSlotWidget* ItemSlot = CreateWidget<UShopItemSlotWidget>(this, ItmeSlotClass);
		if (!ItemSlot) continue;

		if (i < ItemCount)
		{
			// 슬롯 위젯 기본 세팅
			ItemSlot->Init(TypeMatchedShopItems[i], TypeMatchedItems[i]);

			// 슬롯 위젯 델리게이트 바인딩
			ItemSlot->OnItemHovered.AddDynamic(this, &UShopWidget::HandleHovered);
			ItemSlot->OnItemUnhovered.AddDynamic(this, &UShopWidget::HandleUnhovered);
			ItemSlot->OnItemClicked.AddDynamic(this, &UShopWidget::HandleClicked);
			ItemSlot->OnItemHoldProgress.AddDynamic(this, &UShopWidget::HandleHoldProgress);
			ItemSlot->OnItemHoldCompleted.AddDynamic(this, &UShopWidget::HandleHoldCompleted);
		}
		else
		{
			ItemSlot->ClearSlot();
		}

		ItemListPanel->AddChild(ItemSlot);
	}

	//for (const FShopItem& Item : InItems)
	//{
	//	// 아이템 데이터 데이터 테이블에서 불러오기
	//	const FItemData* Data = Inventory->GetItemData(Item.ItemID);
	//	if (!Data) return;

	//	if (Data->ItemType != InType) continue;

	//	// 아이템 슬롯 위젯 생성
	//	UShopItemSlotWidget* ItemSlot = CreateWidget<UShopItemSlotWidget>(this, ItmeSlotClass);
	//	if (!ItemSlot) continue;

	//	// 슬롯 위젯 기본 세팅
	//	ItemSlot->Init(Item, *Data);

	//	// 슬롯 위젯 델리게이트 바인딩
	//	ItemSlot->OnItemHovered.AddDynamic(this, &UShopWidget::HandleHovered);
	//	ItemSlot->OnItemUnhovered.AddDynamic(this, &UShopWidget::HandleUnhovered);
	//	ItemSlot->OnItemClicked.AddDynamic(this, &UShopWidget::HandleClicked);
	//	ItemSlot->OnItemHoldProgress.AddDynamic(this, &UShopWidget::HandleHoldProgress);
	//	ItemSlot->OnItemHoldCompleted.AddDynamic(this, &UShopWidget::HandleHoldCompleted);

	//	// 슬롯 위젯 아이템 패널에 추가
	//	ItemListPanel->AddChild(ItemSlot);
	//}
}

//const FItemDataStruct* UShopWidget::GetItemDataStruct(const FShopItem& ShopItem)
//{
//	if (!ShopItem.ItemDataTable.DataTable) return nullptr;
//
//	return ShopItem.ItemDataTable.DataTable->FindRow<FItemDataStruct>(ShopItem.ItemDataTable.RowName, TEXT("ShopItem Lookup"));
//}

int32 UShopWidget::GetMaxPurchaseCount()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return 0;

	UABCharacterStatComponent* PlayerStat = Cast<UABCharacterStatComponent>(Player->CharacterStatManager);
	if (!PlayerStat) return 0;

	int32 PlayerOwnedGold = PlayerStat->GetSouls();
	int32 CurrentItemPrice = FocusedItem->Price;
	int32 MaxPurchaseCount;

	if (CurrentItemPrice != 0)
	{
		MaxPurchaseCount = PlayerOwnedGold / CurrentItemPrice;
	}
	else
	{
		MaxPurchaseCount = 99;
	}

	return MaxPurchaseCount;
}
