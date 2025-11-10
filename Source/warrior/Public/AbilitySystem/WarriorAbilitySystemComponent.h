// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "GameplayAbilitySpecHandle.h"

#include "WarriorAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	// 무기에 등록된 어빌리티들 등록하는 함수
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbility(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,
		int32 ApplyLevel,
		TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	// 등록된 어빌리티 삭제하는 함수
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	// 특정 GameplayTag를 가진 어빌리티를 찾아서 실행하려는 함수
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
