// WonUngJu All Rights Reserved


#include "WarriorGameInstance.h"

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
	for (const FWarriorGameLevelSet GameLevelSet : GameLevelSets)
	{
		if (!GameLevelSet.IsValid()) continue;

		if (GameLevelSet.LevelTag == InTag)
		{
			return GameLevelSet.Level;
		}
	}

	// 못 찾았을 경우 비어있는 객체 포인터를 반환
	return TSoftObjectPtr<UWorld>();
}
