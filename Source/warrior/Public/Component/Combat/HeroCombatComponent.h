// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Component/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"

/**
 * 
 */

class AWarriorHeroWeapon;

UCLASS()
class WARRIOR_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	// 들고다니는 무기 
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;

	// 현재 장착중인 무기
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCurrentEquippedWeapon() const;

	// 현재 장착중인 무기의 데미지 값 
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	float GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const;

	virtual void OnHitTargetActor(AActor* HitActor) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;
};
