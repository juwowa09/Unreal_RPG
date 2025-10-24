// WonUngJu All Rights Reserved


#include "Component/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"


#include "WarriorDebugHelper.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

// 무기 델리게이트에 등록할 콜백함수
void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	Super::OnHitTargetActor(HitActor);

	if (OverlappedActors.Contains(HitActor)) return;	// 이미 맞은 녀석은 적용 X

	// FGameplayEventData는 GAS 이벤트를 전달할 때 사용
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();	// 주체자
	Data.Target = HitActor;		// 피격자
	
	OverlappedActors.AddUnique(HitActor);		// 맞은 액터 중복처리 X

	// 특정 Actor 가 가지고있는 ASC에게 Event 전달
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(	
		GetOwningPawn(),	// 이 액터가 GAS 지녀야 함
		WarriorGamePlayTags::Shared_Event_MeleeHit,	//	액터의 GAS 에 Tag 이벤트 전달 (특정 Ability가 Wait Event 중)
		Data
	);
}

// 무기 델리게이트에 등록할 콜백함수
void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	Super::OnWeaponPulledFromTargetActor(InteractedActor);

	
}
