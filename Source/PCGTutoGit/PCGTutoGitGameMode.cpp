// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCGTutoGitGameMode.h"
#include "PCGTutoGitCharacter.h"
#include "UObject/ConstructorHelpers.h"

APCGTutoGitGameMode::APCGTutoGitGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
