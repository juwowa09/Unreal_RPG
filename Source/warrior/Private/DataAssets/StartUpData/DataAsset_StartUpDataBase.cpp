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

	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			//	CDO = class default object
			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();

			// CDO 를 바로 적용시 엔진 내부에서 Spec Handle 자동 생성후 적용
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()	// 이펙트의 컨텍스트 정보 생성함수(시전자, 위치등)
			);
		}
	}
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

		// 어빌리티 인스턴스(스펙)에 넣어서 변수를 만들고
		FGameplayAbilitySpec AbilitySpec(Ability);
		
		// 등록할 아바타를 어빌리티 시스템을 통해 구해오기
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		// 어빌리티 시스템에 어빌리티를 등록하도록 함, 기본 제공 함수, 등록시 자동으로 어빌리티의 GiveOn 함수 실행
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
