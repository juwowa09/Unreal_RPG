// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "WarriorSurvivalGameMode.generated.h"

class AWarriorEnemyCharacter;

UENUM(BlueprintType)
enum class EWarriorSurvivalGameModeState : uint8
{
	WaitSpawnNewWave,
	SpanwningNewWave,
	InProgress,
	WaveCompleted,
	AllWavesDone,
	PlayerDied
};

// 한 종류의 Enemy 를 몇마리 소환할지 정의하는 구조체
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

// 하나의 웨이브에 얼마나 많은 적을 생성할지 정의하는 구조체
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisWave = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameModeStateChangedDelegate, EWarriorSurvivalGameModeState, CurrentState);

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorSurvivalGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

private:
	// 현재 게임모드 상태를 변경하는 Setter 함수
	void SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState);

	// 모든 Wave가 끝남을 확인하는 함수
	bool HasFinishedAllWaves() const;

	// 다음 웨이브의 적들은 비동기로드로 미리 로드해오기
	void PreLoadNextWaveEnemies();

	// 현재 Wave 의 DataTable Row 를 가져오는 함수
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;

	// Enemy 스폰 시도하는 함수
	int32 TrySpawnWaveEnemies();

	// Spawn 계속 시도해야하는지 (스폰 개수 숫자 관리) 판단
	bool ShouldKeepSpawnEnemies() const;

	UFUNCTION()
	void OnEnemyDestroyed( AActor* DestroyedActor );
	
	UPROPERTY()
	EWarriorSurvivalGameModeState CurrentSurvivalGameModeState;

	// 현재 Game Mode 가 변경되었음을 알려주는 델리게이트
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSurvivalGameModeStateChangedDelegate OnSurvivalGameModeStateChanged;
	
	// Data Table 을 통해 GameMode 정하
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	// 전체 Wave 숫자
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 TotalWavesToSpawn;
	
	// 현재 웨이브 카운트하는 변수
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveCount = 1;

	// 현재 총 spawn 한 enemy 수
	UPROPERTY()
	int32 CurrentSpawnedEnemiesCounter = 0;
	// 현재 wave 에서 스폰한 enemy 수
	UPROPERTY()
	int32 TotalSpawnedEnemiesThisWaveCounter = 0;
	// TargetPoint 를 저장할 배열
	TArray<AActor*> TargetPointsArray;
	
	// 경과 시간 
	UPROPERTY()
	float TimePassedSinceStart = 0.f;
	
	// Wait 시작 전 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnNewWaveWaitTime = 5.f;

	// 스폰 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemiesDelayTime = 2.f;

	// Wave 끝난 후 대기시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float WaveCompletedWaitTime = 5.f;

	// SoftPtr 과 UClass 를 매핑시킴
	UPROPERTY()
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>, UClass*> PreLoadedEnemyClassMap;

public:
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister);
};
