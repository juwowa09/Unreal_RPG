// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorGamePlayTags.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// CurrentActorInfo는 현재 캐릭터의 정보, AvatarActor는 말 그대로 Pawn
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

// Effect 생성 함수
FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);

	// SpecHandle을 만들기 위한 Context 만들고, 해당 Handle에 필요한 정보들 입력(현재 어빌리티를 통해 생성되는 Effect, 소유, 행위자 정보)
	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	// SpecHandle 만들기 == ASC를 통해서 Effect의 Spec을 만들고 Spec을 가리키는 Handle을 만들고 반환.
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
	
	// 생성된 Effect 가 가지는 데미지 수치를 태그를 통해 설정
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		WarriorGamePlayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	// 태그(키)와 데미지값을 매핑
	if (InCurrentAttackTypeTag.IsValid())
	{
		// 공격 타입별로 다른 데미지를 주기 위해서 현재 공격 태그를 입력함. 
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}
	return EffectSpecHandle;
}
