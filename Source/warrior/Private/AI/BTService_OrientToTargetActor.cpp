// WonUngJu All Rights Reserved


#include "AI/BTService_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	// BT 로 부터 서비스가 Notify 를 받을 수 있도록 초기화 (현재 노드의 활성화/비활성화/틱 이벤트) 
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	RotationInterSpeed = 5.f;
	// 모든 프레임
	Interval = 0.f;
	bNotifyTick = true; // 명시적으로 Tick 허용 0.f 는 없다고 판단함
	RandomDeviation = 0.f;

	// 이 서비스의 키 세렉터 가 어떤 타입의 블랙보드 키만 허용할 것인지 현재는 AActor 만 허용
	InTargetActorKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey),	// 멤버를 문자열로 쓰지 않고 참조로 쓴다?
		AActor::StaticClass()
		);
}

// 서비스가 어떤 블랙보드 키를 참조할지 초기화 하는 함수 (드롭다운이 가능하게 함)
void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 현재 사용중인 블랙보드를 가져옴
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

// 현재 노드를 설명하는 함수
FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient rotation to %s key %s"), *KeyDescription, *GetStaticServiceDescription());
}

// 서비스 실행되는 동안 주기적으로 수행되는 함수 
void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// BlackboardComponent.h
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	// AI Controller.h
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn && TargetActor)
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterSpeed);

		OwningPawn->SetActorRotation(TargetRot);
	}
}
