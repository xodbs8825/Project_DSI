// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "BaseCharacter.h"
#include "InventoryManager.h"

// Sets default values
ABaseItem::ABaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/Blueprints/Props/Sphere.Sphere"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	Box->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
	Box->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnEndOverlap);
}

void ABaseItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("%s Overlap"), *this->GetFName().ToString()));

	if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		PlayerCharacter->IsOverlappingInteractableObject = true;
	}
}

void ABaseItem::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("%s End Overlap "), *this->GetFName().ToString()));

	if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		if (IsActorBeingDestroyed())
		{
		}
		else
		{
		}
	}
}
