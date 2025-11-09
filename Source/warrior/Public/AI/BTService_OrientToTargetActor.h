// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTService_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()

	UBTService_OrientToTargetActor();
	
	//~ Begin UBTNode Interface, Behavior Tree 에셋 로드시 블랙보드의 키와 연결하기 위해
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	// 현재 노드 설명 디스크립트 
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 블랙보드로 부터 참조할 Key
	UPROPERTY(EditAnywhere, Category="Target")
	FBlackboardKeySelector InTargetActorKey;

	// 회전 스피드
	UPROPERTY(EditAnywhere, Category="Target")
	float RotationInterSpeed;
};
