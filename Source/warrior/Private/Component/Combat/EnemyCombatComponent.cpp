// WonUngJu All Rights Reserved


#include "Component/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// 하나만 적용하기 위해 (중복 적용 X)
	if (OverlappedActors.Contains(HitActor)) return;
	
	OverlappedActors.AddUnique(HitActor);

	// TODO:: Implement block check
	bool bIsValidBlock = false;

	// Hit 당한 액터가 Blocking 태그를 가지는 중인지 체크
	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor,
		WarriorGamePlayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		//TODO::check if the block is valid
		// 블록 판정
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock)
	{
		//TODO::Handle successful block
		// 블록 한 경우 (막은 경우)
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGamePlayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		// Player 가 막지 못한 경우 (애너미에 Event 송신) 
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGamePlayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}
