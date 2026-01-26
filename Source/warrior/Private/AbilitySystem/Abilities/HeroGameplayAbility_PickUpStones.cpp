// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStones.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Items/PickUps/WarriorStoneBase.h"
#include "Component/UI/HeroUIComponent.h"

#include "WarriorDebugHelper.h"

void UHeroGameplayAbility_PickUpStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 현재 어빌리티 활성화 시 UI widget 활성화
	// GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUpStones::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// 어빌리티 끝날때 UI widget 비활
	// GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 어빌리티가 실행되어있는 동안 계속해서 호출되는 함수
void UHeroGameplayAbility_PickUpStones::CollectStones()
{
	CollectedStones.Empty();
	TArray<FHitResult> TraceHits;

	// Box Trace 를 통해서 주위에 Channel 에 해당하는 오브젝트가 존재하는지 아닌지 정하기
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + -GetHeroCharacterFromActorInfo()->GetActorUpVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		(-GetHeroCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
		StoneTraceChannel,	// 특정 채널 안에서 Trace
		false,
		TArray<AActor*>(), // 무시할 Actor 목록 현재 무시하는게 없음
		bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		TraceHits,
		true
		);
		//ignoreSelf (마지막 매개변수) 가 true 이면, 액터자신은 Trace 에 포함되지 않음

	for (const FHitResult& TraceHit : TraceHits)
	{
		// 캐스트로 Stone 타입만 받아올 수 있게 함
		if (AWarriorStoneBase* FoundStone = Cast<AWarriorStoneBase>(TraceHit.GetActor()))
			CollectedStones.AddUnique(FoundStone);
	}

	// Stones 이 없으면 사라짐
	if (CollectedStones.IsEmpty())
	{
		// 범위 안에 없으면 현재 어빌리티 취소
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
	
}

void UHeroGameplayAbility_PickUpStones::ConsumeStones()
{
	// 검색할 어빌리티가 없어지면 사라짐
	if (CollectedStones.IsEmpty())
	{
		// 범위 안에 없으면 현재 어빌리티 취소
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	// 한번에 모든 Stone 을 다 소비
	for (AWarriorStoneBase* CollectedStone : CollectedStones)
	{
		if (CollectedStone)
		{
			// Stone 을 통해 소비하도록 함
			CollectedStone->Consume(GetWarriorAbilitySystemComponentFromActorInfo(),GetAbilityLevel());
		}
	}
}
