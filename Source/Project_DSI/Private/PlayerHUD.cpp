// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "BaseCharacter.h"
#include "ABCharacterStatComponent.h"
#include "InventoryManager.h"
#include "HUDHotkeySlotWidget.h"
#include "BaseGameInstance.h"
#include "DSI_SaveGame.h"
#include "ItemEntryWidget.h"
#include "Components/SizeBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UABCharacterStatComponent* Stat = Player->CharacterStatManager;
	if (!Stat) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	Stat->OnSoulsChanged.BindUObject(this, &UPlayerHUD::UpdateSouls);
	Stat->OnSoulsChanged.Execute(Stat->GetSouls());

	Stat->OnSoulsDropped.BindUObject(this, &UPlayerHUD::UpdateDroppesSouls);
	Stat->OnSoulsDropped.Execute(0, false);

	Stat->OnHPChanged.AddDynamic(this, &UPlayerHUD::UpdateHP);
	Stat->OnStaminaChanged.AddDynamic(this, &UPlayerHUD::UpdateStamina);

	//Stat->OnSetHPStaminaSize.AddDynamic(this, &UPlayerHUD::SetHPStaminaSize);

	//Stat->OnSetHPStaminaSize.Broadcast(Stat->MaxHP, Stat->GetMaxVigorHP(), Stat->MaxStamina, Stat->GetMaxEnduranceStamina());
	Stat->OnHPChanged.Broadcast(Stat->CurrentHP, Stat->MaxHP);

	Inventory->OnItemObtained.AddDynamic(this, &UPlayerHUD::CreateItemEntry);

	ClearAllHotkeySlots();

	//LowerHotkey->SetVisibility(ESlateVisibility::Hidden);
	//UpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//NextUpperHotkey->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::RefreshHotkey()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (!Player) return;

	UInventoryManager* Inventory = Player->InventoryManager;
	if (!Inventory) return;

	ClearAllHotkeySlots();

	TMap<EEquipmentSlotType, FInventoryItem> Hotkeys = Inventory->GetAllHotkeyItems();

	for (auto Hotkey : Hotkeys)
	{
		if (Hotkey.Key == EEquipmentSlotType::Hotkey_1)
		{
			Hotkey_1->SetSlot(this, Hotkey.Value);
		}
		else if (Hotkey.Key == EEquipmentSlotType::Hotkey_2)
		{
			Hotkey_2->SetSlot(this, Hotkey.Value);
		}
		else if (Hotkey.Key == EEquipmentSlotType::Hotkey_3)
		{
			Hotkey_3->SetSlot(this, Hotkey.Value);
		}
		else if (Hotkey.Key == EEquipmentSlotType::Hotkey_4)
		{
			Hotkey_4->SetSlot(this, Hotkey.Value);
		}
		else if (Hotkey.Key == EEquipmentSlotType::Hotkey_5)
		{
			Hotkey_5->SetSlot(this, Hotkey.Value);
		}
	}

	//int32 Num = Hotkeys.Num();
	//if (Num == 0)
	//{
	//	ClearAllHotkeySlots();

	//	CurrentType = EEquipmentSlotType::Weapon;
	//	CurrentHotkeyIndex = 0;

	//	LowerHotkey->SetVisibility(ESlateVisibility::Hidden);
	//	UpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//	NextUpperHotkey->SetVisibility(ESlateVisibility::Hidden);

	//	return;
	//}

	//TArray<EEquipmentSlotType> Keys;
	//Hotkeys.GenerateKeyArray(Keys);
	//Keys.Sort();

	//ClearAllHotkeySlots();

	//if (Num == 1)
	//{
	//	if (CurrentType != Keys[0])
	//	{
	//		CurrentType = Keys[0];
	//	}

	//	LowerHotkey->SetVisibility(ESlateVisibility::Hidden);
	//	UpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//	NextUpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//}

	//if (Keys.Contains(CurrentType))
	//{
	//	CurrentHotkeyIndex = Keys.Find(CurrentType);
	//}
	//else
	//{
	//	if (CurrentHotkeyIndex >= Num)
	//	{
	//		CurrentHotkeyIndex = 0;
	//		CurrentType = EEquipmentSlotType::Hotkey_1;
	//	}
	//	else
	//	{
	//		CurrentType = Keys[CurrentHotkeyIndex];
	//	}
	//}

	//int32 PrevIndex = (CurrentHotkeyIndex - 1 + Num) % Num;
	//int32 NextIndex1 = (CurrentHotkeyIndex + 1) % Num;
	//int32 NextIndex2 = (CurrentHotkeyIndex + 2) % Num;

	//if (Num >= 1)
	//{
	//	CurrentHotkey->SetSlot(this, *Hotkeys.Find(Keys[CurrentHotkeyIndex]));
	//}

	//if (Num == 2)
	//{
	//	UpperHotkey->SetSlot(this, *Hotkeys.Find(Keys[NextIndex1]));

	//	LowerHotkey->SetVisibility(ESlateVisibility::Hidden);
	//	UpperHotkey->SetVisibility(ESlateVisibility::Visible);
	//	NextUpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//}
	//else if (Num == 3)
	//{
	//	LowerHotkey->SetSlot(this, *Hotkeys.Find(Keys[PrevIndex]));
	//	UpperHotkey->SetSlot(this, *Hotkeys.Find(Keys[NextIndex1]));

	//	LowerHotkey->SetVisibility(ESlateVisibility::Visible);
	//	UpperHotkey->SetVisibility(ESlateVisibility::Visible);
	//	NextUpperHotkey->SetVisibility(ESlateVisibility::Hidden);
	//}
	//else if (Num >= 4)
	//{
	//	LowerHotkey->SetSlot(this, *Hotkeys.Find(Keys[PrevIndex]));
	//	UpperHotkey->SetSlot(this, *Hotkeys.Find(Keys[NextIndex1]));
	//	NextUpperHotkey->SetSlot(this, *Hotkeys.Find(Keys[NextIndex2]));

	//	LowerHotkey->SetVisibility(ESlateVisibility::Visible);
	//	UpperHotkey->SetVisibility(ESlateVisibility::Visible);
	//	NextUpperHotkey->SetVisibility(ESlateVisibility::Visible);
	//}
}

void UPlayerHUD::NextHotkey(UInventoryManager* Inventory)
{
	TMap<EEquipmentSlotType, FInventoryItem> Hotkeys = Inventory->GetAllHotkeyItems();

	int32 Num = Hotkeys.Num();
	if (Num <= 1) return;

	TArray<EEquipmentSlotType> Keys;
	Hotkeys.GenerateKeyArray(Keys);
	Keys.Sort();

	CurrentHotkeyIndex = (CurrentHotkeyIndex + 1) % Num;
	CurrentType = Keys[CurrentHotkeyIndex];

	RefreshHotkey();
}

void UPlayerHUD::PrevHotkey(UInventoryManager* Inventory)
{
	TMap<EEquipmentSlotType, FInventoryItem> Hotkeys = Inventory->GetAllHotkeyItems();

	int32 Num = Hotkeys.Num();
	if (Num <= 1) return;

	TArray<EEquipmentSlotType> Keys;
	Hotkeys.GenerateKeyArray(Keys);
	Keys.Sort();

	CurrentHotkeyIndex = (CurrentHotkeyIndex - 1 + Num) % Num;
	CurrentType = Keys[CurrentHotkeyIndex];

	RefreshHotkey();
}

void UPlayerHUD::ClearAllHotkeySlots()
{
	Hotkey_1->ClearSlot();
	Hotkey_2->ClearSlot();
	Hotkey_3->ClearSlot();
	Hotkey_4->ClearSlot();
	Hotkey_5->ClearSlot();
}

void UPlayerHUD::UpdateHP(float CurrentHP, float MaxHP)
{
	check(HPBar);

	HPBar->SetPercent(CurrentHP / MaxHP);

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->HP = CurrentHP;

		GI->MarkDirty();
	}
}

void UPlayerHUD::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	check(StaminaBar);

	StaminaBar->SetPercent(CurrentStamina / MaxStamina);
}

void UPlayerHUD::UpdateSouls(int32 Souls)
{
	CurrentSoulsText->SetText(FText::AsNumber(Souls));

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	if (GI && GI->CurrentSave)
	{
		GI->CurrentSave->Souls = Souls;

		GI->MarkDirty();
	}
}

void UPlayerHUD::UpdateDroppesSouls(int32 DroppedSouls, bool IsSoulsDropped)
{
	DroppedSoulsText->SetText(FText::AsNumber(DroppedSouls));
	DroppedSoulsText->SetVisibility(IsSoulsDropped ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UPlayerHUD::CreateItemEntry(const FItemData& ItemData, int32 Count)
{
	if (!ItemEntryPanel || !ItemEntryWidgetClass) return;

	UItemEntryWidget* Entry = CreateWidget<UItemEntryWidget>(this, ItemEntryWidgetClass);
	if (!Entry) return;

	Entry->SetItemEntry(ItemData, Count);

	//ItemEntryPanel->InsertChildAt(0, Entry);
}

//void UPlayerHUD::SetHPStaminaSize(float CurrentHP, float VigorMaxHP, float CurrentStamina, float EnduranceMaxStamina)
//{
//	float HPSize = 1000.0f * (CurrentHP / VigorMaxHP);
//	float StaminaSize = 1000.0f * (CurrentStamina / EnduranceMaxStamina);
//
//	HPSizeBox->SetWidthOverride(HPSize);
//	StaminaSizeBox->SetWidthOverride(StaminaSize);
//}
