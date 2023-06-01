// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDPMultiplayerGameMode.h"
#include "PDPMultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

APDPMultiplayerGameMode::APDPMultiplayerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
