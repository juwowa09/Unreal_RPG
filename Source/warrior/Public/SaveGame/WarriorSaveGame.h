// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WarriorTypes/WarriorEnumTypes.h"

#include "WarriorSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	// 게임 난이도 저장할 변수
	UPROPERTY(BlueprintReadOnly)
	EWarriorGameDifficulty SavedCurrentGameDifficulty;
};
