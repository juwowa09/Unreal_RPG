// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"

#include "WarriorGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FWarriorGameLevelSet
{
	GENERATED_BODY()

	// 레벨의 태그와
	UPROPERTY(EditDefaultsOnly, meta = (Category="GameData.Level"))
	FGameplayTag LevelTag;

	// 레벨 자체를 매핑
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	// 태그와 레벨이 유효 한지 확인
	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Game Instance 초기화 시 한 번 호출되는 함수
	virtual void Init();
	
protected:
	// GameInstance Init 시, Map 을 Load 하기 전 준비하는 Delegate 에 등록할 함수
	virtual void OnPreLoadMap(const FString& MapName);

	// Map 을 Load 하고 난 후 Delegate 에 등록할 함수
	virtual void OnPostLoadMapWithWorld( UWorld* LoadedWorld );
	
	// 레벨과 태그를 배열로 저장
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;

public:
	// 태그를 매개변수로 동일한 태그를 가진 Level 찾기
	UFUNCTION(BlueprintPure, meta=(GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;
};
