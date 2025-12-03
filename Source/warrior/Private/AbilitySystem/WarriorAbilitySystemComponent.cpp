// WonUngJu All Rights Reserved


#include "AbilitySystem/WarriorAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "WarriorGamePlayTags.h"

#include "WarriorDebugHelper.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	// 액티브 가능한 어빌리티중에(등록된 어빌리티)
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 어빌리티 태그가 없으면 다음 어빌리티로
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		// 토글 태그의 인풋의 경우, 어빌리티가 활성화 되어있으면 비활성, 아니면 반대
		if (InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_Toggleable))
		{
			if (AbilitySpec.IsActive())
			{
				CancelAbilityHandle(AbilitySpec.Handle);
			}
			else
			{
				// 기능 활성화
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
		else
		{
			// 토글이 아닌 어빌리티 기능 활성화
			if (AbilitySpec.IsActive())
			{
				FGameplayEventData Data;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					GetAvatarActor(),
					WarriorGamePlayTags::Player_Event_Combo,
					Data
					);
			}
			else
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_MustBeHeld))
		return;

	// 등록된 어빌리티
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 어빌리티 태그가 없으면 다음 어빌리티로
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			// 활성중인 어빌리티 비활성화
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbility(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty()) return;

	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
		// Debug::Print(*GetNameSafe(AbilitySet.AbilityToGrant));

		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()) return;

	for (const FGameplayAbilitySpecHandle SpecHandles : InSpecHandlesToRemove)
	{
		if (SpecHandles.IsValid())
		{
			ClearAbility(SpecHandles);
		}
	}
	
	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	// ASC 가 보유중인 Ability 중에 태그가 일치하는 어빌리티들을 Specs 배열에 담음
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),
		FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		// 랜덤한 어빌리티 하나 골라서
		const int32 RandomAbilityIndex = FMath::RandRange(0,FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			// 실행한다
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}

