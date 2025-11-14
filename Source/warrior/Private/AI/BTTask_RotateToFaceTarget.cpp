// WonUngJu All Rights Reserved


#include "AI/BTTask_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

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

// BT에 현재 Task 에 필요한 Memory Size를 알리는 함수
uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

// Task 시작시점에 한 번 호출
EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 동일한 NodeMemory 주소를 계속 전달함
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	// 8바이트 NodeMemory 포인터를 형 변환
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);

	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	if (!Memory->isValid()) {return EBTNodeResult::Failed;}

	// 이미 각도 안에 들어와있으면 Succeeded (성공, 앞으로 할필요 없음)
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	// 각도가 안맞으므로 진행해야 함 -> InProgress, TickTask 활성화
	return EBTNodeResult::InProgress;
}

// Task Node가 Progress 상태일때 매 프레임 호출
void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 동일한 NodeMemory 주소를 계속 전달함
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	if (!Memory->isValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		// Task 종료
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(),LookAtRot, DeltaSeconds, RotationInterSpeed);
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);

	return AngleDiff <= AnglePrecision;
}