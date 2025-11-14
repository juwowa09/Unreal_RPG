// WonUngJu All Rights Reserved


#include "AI/BTTask_RotateToFaceTarget.h"

#include "BehaviorTree/BlackboardData.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate to Face Target Actor");

	AnglePrecision = 10.f;
	RotationInterSpeed = 5.f;

	// 프레임마다 Tick 함수 호출
	bNotifyTick = true;
	// Task 종료요청 콜백함수허용 (데이터 정리)
	bNotifyTaskFinished = true;
	// AI마다 독립된 인스턴스 노드를 생성할지 ( Task 내부 멤버 변수 사용시 True 해야 멤버 변수를 AI 가 공유하지 않음 )
	bCreateNodeInstance = false;

	// Task Node 초기화
	INIT_TASK_NODE_NOTIFY_FLAGS();

	// Blackboard Key Selector에 ‘Actor 타입만 선택하도록’ 필터
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass,InTargetToFaceKey),AActor::StaticClass());
}

// 블랙보드 에셋을 통해 Key 들을 받아올 수 있도록
void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 블랙보드에서 키 받아와서 드롭다운 할 수 있게 함
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();

	return	FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}