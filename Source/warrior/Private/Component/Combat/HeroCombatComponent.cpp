// WonUngJu All Rights Reserved


#include "Component/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

#include "WarriorDebugHelper.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	Super::OnHitTargetActor(HitActor);

	Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT(" hit ") + HitActor->GetActorNameOrLabel(),FColor::Green);
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	Super::OnWeaponPulledFromTargetActor(InteractedActor);
	
	Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT("'s weapon pulled from ") + InteractedActor->GetActorNameOrLabel(),FColor::Red);
}
