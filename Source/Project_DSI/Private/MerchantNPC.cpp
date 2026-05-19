// Fill out your copyright notice in the Description page of Project Settings.


#include "MerchantNPC.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.h"

AMerchantNPC::AMerchantNPC()
{
	InteractionText = FText::FromString(TEXT("Open Shop"));
}

void AMerchantNPC::Interact_Implementation(APlayerController* InteractingController)
{
	if (!InteractingController) return;

	if (AABPlayerController* PC = Cast<AABPlayerController>(InteractingController))
	{
		// »óÁ¡ UI ¿ÀÇÂ
		PC->OpenShopUI(this);
	}
}

const FItemData* AMerchantNPC::GetItemData(FName ItemID) const
{
	if (!ItemDataTable) return nullptr;

	return ItemDataTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));
}