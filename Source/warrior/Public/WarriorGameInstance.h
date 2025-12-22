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

protected:
	// 레벨과 태그를 배열로 저장
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;

public:
	// 태그를 매개변수로 동일한 태그를 가진 Level 찾기
	UFUNCTION(BlueprintPure, meta=(GameplayTagFilter = "GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;
};
