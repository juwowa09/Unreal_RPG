// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"

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

bool UWarriorGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	// Define a common lambda to check for blocked tags
	bool bBlocked = false;
	auto CheckForBlocked = [&](const FGameplayTagContainer& ContainerA, const FGameplayTagContainer& ContainerB)
	{
		// Do we not have any tags in common?  Then we're not blocked
		// A가 부모, B가 자식 즉 B가 A를 가지고있으면 안됨.
		if (ContainerA.IsEmpty() || ContainerB.IsEmpty() || !ContainerB.HasAny(ContainerA))
		{
			return;
		}

		if (OptionalRelevantTags)
		{
			// Ensure the global blocking tag is only added once
			if (!bBlocked)
			{
				UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
				const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
				OptionalRelevantTags->AddTag(BlockedTag);
			}

			// Now append all the blocking tags
			OptionalRelevantTags->AppendMatchingTags(ContainerA, ContainerB);
		}

		bBlocked = true;
	};

	// Define a common lambda to check for missing required tags
	bool bMissing = false;
	auto CheckForRequired = [&](const FGameplayTagContainer& TagsToCheck, const FGameplayTagContainer& RequiredTags)
	{
		// Do we have no requirements, or have met all requirements?  Then nothing's missing
		if (RequiredTags.IsEmpty() || TagsToCheck.HasAll(RequiredTags))
		{
			return;
		}

		if (OptionalRelevantTags)
		{
			// Ensure the global missing tag is only added once
			if (!bMissing)
			{
				UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
				const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
				OptionalRelevantTags->AddTag(MissingTag);
			}

			FGameplayTagContainer MissingTags = RequiredTags; 
			MissingTags.RemoveTags(TagsToCheck.GetGameplayTagParents());
			OptionalRelevantTags->AppendTags(MissingTags);
		}

		bMissing = true;
	};

	// Start by checking all of the blocked tags first (so OptionalRelevantTags will contain blocked tags first)
	CheckForBlocked(AbilitySystemComponent.GetBlockedAbilityTags(), GetAssetTags());
	CheckForBlocked(AbilitySystemComponent.GetOwnedGameplayTags(), ActivationBlockedTags);
	if (SourceTags != nullptr)
	{
		CheckForBlocked(*SourceTags, SourceBlockedTags);
	}
	if (TargetTags != nullptr)
	{
		CheckForBlocked(*TargetTags, TargetBlockedTags);
	}

	// Now check all required tags
	CheckForRequired(AbilitySystemComponent.GetOwnedGameplayTags(), ActivationRequiredTags);
	if (SourceTags != nullptr)
	{
		CheckForRequired(*SourceTags, SourceRequiredTags);
	}
	if (TargetTags != nullptr)
	{
		CheckForRequired(*TargetTags, TargetRequiredTags);
	}

	// We succeeded if there were no blocked tags and no missing required tags	
	return !bBlocked && !bMissing;
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

	// 플레이어 ASC 가 실제로 Target ASC에 Effect 를 적용시키고 Active 중인 EffectSpecHandle을 리턴
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

void UWarriorGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle,
	const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())
		return;

	// 현재 어빌리티를 가지고 있는 Pawn
	APawn* OwningPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	// 모든 Hit Target 에 대해서 수행
	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			// Hit Pawn 이 적일 경우
			if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				// 유요하게 적용 됐을 경우
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						WarriorGamePlayTags::Shared_Event_HitReact,
						Data
						);
				}
			}
		}
	}
}
