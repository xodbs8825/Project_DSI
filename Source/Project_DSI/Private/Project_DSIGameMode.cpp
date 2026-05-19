// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_DSIGameMode.h"
#include "Project_DSICharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_DSIGameMode::AProject_DSIGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
