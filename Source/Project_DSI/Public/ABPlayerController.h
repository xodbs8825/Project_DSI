// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManager.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

class UInventoryWidget;
class UShopWidget;
class UBonfireWidget;
class ULevelUpWidget;
class UEquipmentViewWidget;
class UEquipItemWidget;
class UDeathWidget;

class UInteractionComponent;
class UInteractPromptWidget;

class AMerchantNPC;
class ABonfire;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIOpened, AABPlayerController*, PC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIClosed, AABPlayerController*, PC);

/**
 * 
 */
UCLASS()
class PROJECT_DSI_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

public:
	void OpenInventoryUI(class UInventoryManager* Inventory);
	void CloseInventoryUI();

	void OpenShopUI(AMerchantNPC* MerchantNPC);
	void CloseShopUI();

	void OpenBonfireUI(FName BonfireName);
	void CloseBonfireUI();

	void OpenLevelUpUI();
	void CloseLevelUpUI();

	void OpenEquipmentUI(UInventoryManager* Inventory);
	void CloseEquipmentUI();

	void OpenEquipItemUI(UInventoryManager* Inventory, EEquipmentSlotType InSlotType);
	void CloseEquipItemUI(UInventoryManager* Inventory);

	void CloseAllWidget();

	bool IsInventoryUIValid() const { return InventoryWidget != nullptr; }
	bool IsShopUIValid() const { return ShopWidget != nullptr; }
	bool IsBonfireUIValid() const { return BonfireWidget != nullptr; }
	bool IsLevelUpUIValid() const { return LevelUpWidget != nullptr; }
	bool IsEquipmentUIValid() const { return EquipmentWidget != nullptr; }
	bool IsEquipItemUIValid() const { return EquipItemWidget != nullptr; }

	bool IsInventoryUIOpen() const;
	bool IsShopUIOpen() const;
	bool IsBonfireUIOpen() const;
	bool IsLevelUpUIOpen() const;
	bool IsEquipmentUIOpen() const;
	bool IsEquipItemUIOpen() const;

	bool IsOtherWidgetOpen(UUserWidget* Widget) const;

	void CreateDeathWidget();
	void TriggerDeathWidget();

	UFUNCTION(BlueprintCallable)
	void SetInputMappingContext(class UInputMappingContext* IMC);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnInteractTargetChanged(AActor* NewTarget);

private:
	void SetUIInputMode(class UUserWidget* Widget, bool IsUIOpened);

public:
	FOnUIOpened OnUIOpened;
	FOnUIClosed OnUIClosed;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UShopWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBonfireWidget> BonfireWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULevelUpWidget> LevelUpWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipmentViewWidget> EquipmentWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipItemWidget> EquipItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeathWidget> DeathWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UFadeWidget> FadeWidgetClass;

private:
	UPROPERTY(EditAnywhere)
	class AABPlayerState* ABPlayerState;

	UPROPERTY()
	UInventoryWidget* InventoryWidget;

	UPROPERTY()
	UShopWidget* ShopWidget;

	UPROPERTY()
	UBonfireWidget* BonfireWidget;

	UPROPERTY()
	ULevelUpWidget* LevelUpWidget;

	UPROPERTY()
	UEquipmentViewWidget* EquipmentWidget;

	UPROPERTY()
	UEquipItemWidget* EquipItemWidget;

	UPROPERTY()
	UInteractionComponent* InteractionComponent;

	UPROPERTY()
	UDeathWidget* DeathWidget;

	AActor* PreviousTarget;

	TArray<UUserWidget*> Widgets;

	FName CurrentBonfireName;
};
