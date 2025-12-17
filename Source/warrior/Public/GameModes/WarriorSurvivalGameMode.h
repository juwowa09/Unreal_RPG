// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "WarriorSurvivalGameMode.generated.h"

UENUM(BlueprintType)
enum class EWarriorSuvivalGameModeState : uint8
{
	WaitSpawnNewWave,
	SpanwningNewWave,
	InProgress,
	WaveCompleted,
	AllWavesDone,
	PlayerDied
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameModeStateChangedDelegate, EWarriorSuvivalGameModeState, CurrentState);

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

private:
	// 현재 게임모드 상태를 변경하는 Setter 함수
	void SetCurrentSurvivalGameModeState(EWarriorSuvivalGameModeState InState);
	
	UPROPERTY()
	EWarriorSuvivalGameModeState CurrentSurvivalGameModeState;

	// 현재 Game Mode 가 변경되었음을 알려주는 델리게이트
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnSurvivalGameModeStateChangedDelegate OnSurvivalGameModeStateChanged;
};