// WonUngJu All Rights Reserved


#include "GameModes/WarriorSurvivalGameMode.h"
#include "Engine/AssetManager.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid datat table in survival game mode blueprint"));

	SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);

	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	// 다음 웨이브 미리 로드해오기
	PreLoadNextWaveEnemies();
}

// Tick 활성화 해야하는 진행되는 함수
void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 Wave Spawn 대기상태
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

	// 현재 Wave Spawn 중 상태
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::SpanwningNewWave)
	{
		TimePassedSinceStart += DeltaTime;

		// 대기시간 지나면 GameMode 상태 바꾸기
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			// 적 스폰 시도, Spawn 한 만큼 Count
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			TimePassedSinceStart = 0.f;

			// Wave 진행중인 상태로
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}

	// 현재 Wave 가 완료되었을 경우
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;
		
		// 대기시간 지나면 GameMode 상태 바꾸기
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;
			// 웨이브 카운트 증가
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
				// 넘어가기 전 Next 웨이브 적 미리 로드하기
				PreLoadNextWaveEnemies();
			}
		}
	}
}

// 게임 시작시 난이도 설정 함수
void AWarriorSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 게임 시작 시 저장된 난이도를 불러와서 적용함
	EWarriorGameDifficulty SavedGameDifficulty;
	
	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		// 현재 게임모드 난이도 설정하기
		CurrentGameDifficulty = SavedGameDifficulty;
	}
}


bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveEnemies()
{
	// 모든 wave가 끝나버림
	if (HasFinishedAllWaves()) return;

	// 전 Wave 클래스 비우기
	PreLoadedEnemyClassMap.Empty();

	// 데이터 Row 에서 모든 Enemy Info 를 하나씩 가져오기
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// Enemy Info 에 클래스가 비어있으면 넘어가기
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// Enemy Info 에 등록된 클래스들을 하나씩 Load 해서 Map 에 저장
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			// 람다 함수를 통해서 콜백 등록
			// 위의 SoftClass 가 Load 완료되면, Enemy Info 의 클래스와 매핑 
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo, this]()
				{
					// 로드된 클래스 가져오기
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						// Soft 참조와 실제로 로드된 클래스를 매핑
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
					}
				}
				)
			);
	}
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	// 이름을 통해 Data Table 에서 Row 밸류를 져오기
	const FName RowName = FName(TEXT("Wave")+ FString::FromInt(CurrentWaveCount));

	// 이름을 통해 Data Table 에서 Row 밸류 가져오기
	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName,FString());

	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"),*RowName.ToString());
	
	return FoundRow;
}

int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		// 현재 Map 의 Target point 를 모두 가져와 Array 에 할당
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	// Enemy Spawn 시 Rule 적, 충돌 발생시 조정, 그래도 충돌하면 무조건 아무대나 스폰
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 현재 Wave 테이블의 row 에서 소환할 Enemies Info 받아오기
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		// 비어있으면 소환할게 없으므로 넘어가기
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		// Data Table의 Min, Max 값 받아오기
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);
		
		// 클래스에 알맞는 로드된 클래스 찾기
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		// Spawn 할 만큼 반복
		for (int32 i = 0; i < NumToSpawn; i++)
		{
			// TargetPoint 중 아무대서나 소환 가능하도록 랜덤 인덱스
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			// Location 값 받아오기
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			// Forward Vector 를 통해 회전값 만들기
			// To Orientation Rotator 는 현재 벡터를 Orientation 해서 Rotator 를 만듦
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector RandomLocation;
			// SpawnOrigin을 중심으로 반경 400 안에서 NavMesh 위에 있는 ‘이동 가능한 랜덤 위치’를 하나 찾기, Random Location 에 저장
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
				this,
				SpawnOrigin,
				RandomLocation,
				400.f);

			RandomLocation += FVector(0.f,0.f,150.f);
			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass,RandomLocation,SpawnRotation,SpawnParam);
			
			if (SpawnedEnemy)
			{
				// Enemy Destroyed 델리게이트에 함수 등록
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);

				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	
	return EnemiesSpawnedThisTime;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	// 현재 Wave 에서 Spawn 된 개수가 Table 의 최대값보다 크면 안됨
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	// 실시간 스폰 된 적 숫자 카운팅
	CurrentSpawnedEnemiesCounter--;

	// Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter: %i, Total Spawned This Wave: %i"),CurrentSpawnedEnemiesCounter,TotalSpawnedEnemiesThisWaveCounter));

	// 스폰된 숫자가 max 를 넘어가지 않았다면 다시 스폰
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)	// max 넘어갔는데 다죽음 -> 다음 웨이브 실행
	{
		// 다죽었으면 초기화상태로 돌리기 , 다음 Wave 실행
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaveCompleted);
	}
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState)
{
	// 현재 게임모드 변경
	CurrentSurvivalGameModeState = InState;

	// 등록한 컴포넌트들에게 현재 게임모드 상태가 변경됨을 알림
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	// 보스로부터 소환된 Enemy GameMode에 등록하는 함수 
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;

			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}