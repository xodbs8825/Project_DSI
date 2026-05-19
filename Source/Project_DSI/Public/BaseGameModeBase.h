#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameModeBase.generated.h"

UCLASS()
class PROJECT_DSI_API ABaseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABaseGameModeBase();

protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SpawnDroppedItems();
	void SpawnDroppedSouls();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AItemDropActor> ItemDropClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASoulsDropActor> SoulsDropClass;
};
