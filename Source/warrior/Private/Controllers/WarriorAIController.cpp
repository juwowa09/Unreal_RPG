// WonUngJu All Rights Reserved


#include "Controllers/WarriorAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "WarriorDebugHelper.h"
#include "Perception/AIPerceptionComponent.h"

// 기본 PathFollowingComponent 를 군중 회피(Navigation Crowd) 로 변경
AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	// 
	if (UCrowdFollowingComponent* CrowdFollowingComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Debug::Print(TEXT("CrowdFollowingComponent is Valid"), FColor::Green);
	}

	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");

	// 적만 감지, 아군이나 중립대상은 무시
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	// 시야 반경, 각도 설정
	// LoseSightRadius가 0이면  시야를 잃는 순간 바로 감지 해제
	AISenseConfig_Sight->SightRadius = 5000.f;
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

	// 실제로 시야, 청각 등을 관리하는 PerceptionComponent
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");

	// 시야 감지를 등록, Dominant(지배적 감각)
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	// 새로 감지하거나 감지를 잃을 때 여기 등록한 델리게이트를 실행
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
}
