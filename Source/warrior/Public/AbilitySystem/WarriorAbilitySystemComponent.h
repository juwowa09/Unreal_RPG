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

	UFUNCTION(BlueprintCallable, Category="Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbility(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,
		int32 ApplyLevel,
		TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);
	
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
};
