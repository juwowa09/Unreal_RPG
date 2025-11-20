// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Controllers/WarriorHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"

#include "WarriorDebugHelper.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	// 어빌리티 활성시 Target
	TryLockOnTarget();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// 어빌리티 끝날 시 초기화
	CleanUp();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 실제로 Ability Activation 에서 호출, 그리는 함수
void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	// 비어있으면 Target 할 게 없으므로 리턴
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	// 가장가까운 녀석 가져오기
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		// 위젯 그리기
		DrawTargetLockWidget();

		// 위젯 위치 설정
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

// 타겟 락 가능한 적 받아오는 함수
void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	// 결과로 받아올 배열
	TArray<FHitResult> BoxTraceHits;

	// 현재 캐릭터 위치 + 앞방향으로 탐색, 박스를 start -> end 까지 sweep 하면서
	// BoxTrace Channel 안에 있는것들을 충돌했는지 검사
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(), // 시작위치, 끝 위치
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistancce,
		TraceBoxSize / 2.f,		// 박스의 크기 설정
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(), // 박스의 앞면 설정
		BoxTraceChannel,	// 감지할 타입 설정
		false,
		TArray<AActor*>(),		// 무시할 타입 설정 (세부 타입)
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,	// 디버깅 설정
		BoxTraceHits,	 // 결과 
		true		// 자신 무시
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		// 액터일 경우
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

// 가장 가까운 대상 가져오기
AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosetDistance = 0.f;

	// 가장 가까운 Target 하나 가져오기
	return UGameplayStatics::FindNearestActor(
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		InAvailableActors,
		ClosetDistance
	);
}

// 실제로 Target Lock 그리는 부분
void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assgin a valid widget class in Blueprint"));

		// 위젯 create == controller 필
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawnTargetLockWidget);

		// Widget Viewport 에 추가 (그리기)
		DrawnTargetLockWidget->AddToViewport();	
	}
}

// 뷰포트에 Lock Position 위치시키는 함수
void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;

	// Controller -> CameraManager -> Viewport -> CameraComponent 로 이어져서 Viewport 를 받아옴
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true		// 플레이어 Viewport 를 기준으로 그리기
	);

	// 타겟 위치를 왼쪽 모서리 기준으로 Widget 을 그렸기 때문에 반절씩 위치를 빼줘야함
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		// 람다 함수를 쓰는 이유 == 그 자리에서 즉석으로 처리할 작은 함수를 만들기, this 로 현재 객체 내부 변수 접근 가능
		// 내부 모든 위젯에 대해서 람다 함수 실행
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				// 찾은 것만 저장
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);		
	}
	// 제대로 위치 계산해서 적용
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

// 현재 어빌리티 하나만 캔슬하는 함수, true == 서버, 클라 모두에게 캔슬하라 명령
void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		true
		);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	// 어빌리티 끝날때 클린 업
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
}
