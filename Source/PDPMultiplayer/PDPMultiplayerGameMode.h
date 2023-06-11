// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "PDPMultiplayerCharacter.h"
#include "PDPMultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class APDPMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APDPMultiplayerGameMode();

	void Respawn(AController* Controller);
};



