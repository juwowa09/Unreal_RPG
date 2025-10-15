// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkedAnimLayer.generated.h"

/**
 * 
 */
class UWarriorHeroAnimInstance;

UCLASS()
class WARRIOR_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	// 프로퍼티 액세스 하는 유형에 있어야 함
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
	
};
