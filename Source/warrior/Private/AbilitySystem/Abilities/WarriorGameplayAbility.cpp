// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Info -> 스킬사용의 주체, Spec -> 스킬의 주요 정보를 가지는 구조
void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			// Try 함수를 통해 직접 실행하는 구문, ASC에 스펙이 등록되어있지 않으면 실행되지 않는다.
			// Try 함수를 통해 블루프린트의 EventAbility 실행, 어빌리티의 Current Actor를 여기서 지정
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

// Info -> 스킬사용의 주체, Spec -> 스킬의 주요 정보를 가지는 구조
void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	// 현재 스킬을 쓴 아바타(스킬 호출 아바타) 에서 class 컴포넌트를 찾아서 반환
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	// 자체적으로 CurrentActorInfo 라는 변수를 가짐
	// TryActivateAbility() 호출시 세팅되는 변수
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	check(TargetASC && InSpecHandle.IsValid());
	
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}