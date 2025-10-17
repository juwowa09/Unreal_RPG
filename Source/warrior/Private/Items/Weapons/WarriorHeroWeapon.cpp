// WonUngJu All Rights Reserved


#include "Items/Weapons/WarriorHeroWeapon.h"

void AWarriorHeroWeapon::AssignGrantedAbilityHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	GrantedAbilitySpecHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecHandles;
}
