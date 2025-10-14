// WonUngJu All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

#include "WarriorDebugHelper.h"

bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilitytoGrant;
}

void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	for (const FWarriorHeroAbilitySet& Abilityset : HeroStartUpAbilitySets)
	{
		if (!Abilityset.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(Abilityset.AbilitytoGrant);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Abilityset.InputTag);
		
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
