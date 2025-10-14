// WonUngJu All Rights Reserved


#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 어빌리티 태그가 없으면 다음 어빌리티로
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		// 기능 활성화
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}
