// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDPMultiplayerGameMode.h"

#include "PDPMultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
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

void APDPMultiplayerGameMode::Respawn(AController* Controller)
{
	APawn* Pawn = Controller->GetPawn();

	if (IsValid(Pawn))
	{
		Pawn->Destroy();
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

	const APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStarts[FMath::RandHelper(PlayerStarts.Num())]);
	
	Controller->Possess(Cast<APDPMultiplayerCharacter>(World->SpawnActor(DefaultPawnClass, &RandomPlayerStart->GetTransform(), FActorSpawnParameters())));
}
