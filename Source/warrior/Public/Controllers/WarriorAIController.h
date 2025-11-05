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

	// Other 액터가 어느팀인지 확인하는 함수
	//~Begin IGenericTeamAgentInterface Interface.
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~End IGenericTeamAgentInterface Interface.
	
protected:
	virtual void BeginPlay() override;
	
	// 시야, 청각 등을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	// 시야 감지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	// 시야를 통해 무언가를 감지하거나 감지를 잃을때 호출되는 이벤트(Perception 델리게이트에 등록) 
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config")
	bool bEnableDetoureCrowdAvoidance = true;
	
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config", meta = (EditCondition = " bEnableDetoureCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetoueCrowdAvoidanceQuality = 4;
	
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config", meta = (EditCondition = " bEnableDetoureCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
