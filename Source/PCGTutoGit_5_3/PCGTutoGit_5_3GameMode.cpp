// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCGTutoGit_5_3GameMode.h"
#include "PCGTutoGit_5_3Character.h"
#include "UObject/ConstructorHelpers.h"

APCGTutoGit_5_3GameMode::APCGTutoGit_5_3GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
