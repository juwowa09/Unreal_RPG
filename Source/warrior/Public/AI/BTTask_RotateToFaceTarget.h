// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToFaceTarget.generated.h"

// Task Node 전용 NodeMemory == AI 마다 따로 존재
struct FRotateToFaceTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;

	bool isValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTTask_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_RotateToFaceTarget();

	//~Begin UBTNode Interface
	// Black Board 에셋을 통해 노드 초기화
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	//~End UBTNode Interface

	// Task 시작시점 한 번 호출
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// Task Node가 Progress 상태일때 매 프레임 호출
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool HasReachedAnglePrecision(APawn* QueryPawn,AActor* TargetActor) const;

	// 오차 허용 각도 (멈출 각도)
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision;

	// 회전 속도
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterSpeed;

	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetToFaceKey;
	
};
