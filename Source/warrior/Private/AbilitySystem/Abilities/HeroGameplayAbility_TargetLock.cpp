// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "WarriorDebugHelper.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUp();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

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
		Debug::Print(CurrentLockedActor->GetActorNameOrLabel());
	}
	else
	{
		CancelTargetLockAbility();
	}
}

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

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	// 현재 어빌리티 하나만 캔슬하는 함수, true == 서버, 클라 모두에게 캔슬해라
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
}
