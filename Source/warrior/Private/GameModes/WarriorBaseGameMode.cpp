// WonUngJu All Rights Reserved


#include "GameModes/WarriorBaseGameMode.h"

AWarriorBaseGameMode::AWarriorBaseGameMode()
{
	// 게임모드 실행시 Tick 활성화
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}
