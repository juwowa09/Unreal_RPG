// WonUngJu All Rights Reserved


#include "GameModes/WarriorSurvivalGameMode.h"

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid datat table in survival game mode blueprint"));

	SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);

	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();
}

void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Wave Spawn 대기상태
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		
		// 대기시간 지나면 GameMode 상태 바꾸기
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;

			// Wave 스폰중인 상태로
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::SpanwningNewWave);
		}
	}

	// Wave Spawn 중 상태
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::SpanwningNewWave)
	{
		TimePassedSinceStart += DeltaTime;

		// 대기시간 지나면 GameMode 상태 바꾸기
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			// TODO:Handle Spawn new enemies

			TimePassedSinceStart = 0.f;

			// Wave 진행중인 상태로
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}

	// Wave 가 완료되었을 경우
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;
		
		// 대기시간 지나면 GameMode 상태 바꾸기
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;

			CurrentWaveCount++;

			if (HasFinishedAllWaves())
			{
				// 모든 Wave 클리어
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::AllWavesDone);
			}
			else
			{
				// 다음 웨이브로 넘어가기
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
			}
		}
	}
}


bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState)
{
	// 현재 게임모드 변경
	CurrentSurvivalGameModeState = InState;

	// 등록한 컴포넌트들에게 현재 게임모드 상태가 변경됨을 알림
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}
