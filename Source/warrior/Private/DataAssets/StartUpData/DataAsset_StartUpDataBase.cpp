// WonUngJu All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AI/NavigationModifier.h"

// 현재 가지고있는 어빌리티들을 등록하도록 하는 함수
void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
                                                              int32 ApplyLevel)
{
	check(InASCToGive);
	
	GrantAbilities(ActivateOnGiveAbilities, InASCToGive,ApplyLevel);
	GrantAbilities(ReactivateAbilities, InASCToGive,ApplyLevel);
}

// 실제로 등록하는 함수
void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
	UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty()) return;

	// 목록들을 하나하나
	for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability) continue;

		// 어빌리티 명세(스펙)에 넣어서 변수를 만들고
		FGameplayAbilitySpec AbilitySpec(Ability);
		// 등록할 아바타를 어빌리티 시스템을 통해 구해오기
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		// 어빌리티 시스템에 어빌리트를 등록하도록 함
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
