// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "WarriorAIController.generated.h"

class UAIPerceptionComponent;
class UAISense_Sight;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWarriorAIController(const FObjectInitializer& ObjectInitializer);

protected:
	// 시야, 청각 등을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	// 시야 감지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
