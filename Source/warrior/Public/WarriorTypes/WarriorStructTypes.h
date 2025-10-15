// WonUngJu All Rights Reserved

#pragma once

#include "WarriorStructTypes.generated.h"

class UWarriorHeroLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	// 무기마다 고유 애니메이션 레이어를 가짐
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
	
};