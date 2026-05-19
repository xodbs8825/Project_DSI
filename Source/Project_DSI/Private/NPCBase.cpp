// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBase.h"
#include "Components/SphereComponent.h"
#include "InteractPromptWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANPCBase::ANPCBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(200.f);

	InteractPromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractPrompt"));
	InteractPromptComponent->SetupAttachment(GetMesh());

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

	InteractionText = FText::FromString(TEXT("Talk"));
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	if (InteractPromptComponent)
	{
		static const FName HeadSocket(TEXT("Head"));

		if (GetMesh()->DoesSocketExist(HeadSocket))
		{
			InteractPromptComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("Head"));
		}

		InteractPromptComponent->SetVisibility(false);
	}
}

void ANPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!InteractPromptComponent || !InteractPromptComponent->IsVisible()) return;

	// 항상 카메라 보기
	if (APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		FVector ToCamera = Cam->GetCameraLocation() - InteractPromptComponent->GetComponentLocation();

		FRotator LookAtRot = ToCamera.Rotation();
		LookAtRot.Pitch = 0.0f;
		LookAtRot.Roll = 0.0f;

		InteractPromptComponent->SetWorldRotation(LookAtRot);
	}
}

void ANPCBase::Interact_Implementation(APlayerController* InteractingController)
{
}

void ANPCBase::SetInteractPromptVisible_Implementation(bool bVisible)
{
	if (!InteractPromptComponent) return;

	InteractPromptComponent->SetVisibility(bVisible);

	if (bVisible)
	{
		if (UUserWidget* Widget = InteractPromptComponent->GetUserWidgetObject())
		{
			if (UInteractPromptWidget* PromptWidget = Cast<UInteractPromptWidget>(Widget))
			{
				FText Text = IInteractInterface::Execute_GetInteractText(this);
				PromptWidget->SetPromptText(Text);
			}
		}
	}
}

FText ANPCBase::GetInteractText_Implementation() const
{
	return InteractionText;
}

