// WonUngJu All Rights Reserved


#include "GameModes/WarriorSurvivalGameMode.h"

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSuvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;

	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}
