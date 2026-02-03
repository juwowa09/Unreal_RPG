// WonUngJu All Rights Reserved


#include "Items/PickUps/WarriorStoneBase.h"
#include "Characters/WarriorHeroCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorGamePlayTags.h"
#include "GameplayTagContainer.h"
#include "WarriorDebugHelper.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	// 클래스 Effect CDO 를 로드해오기
	UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();

	// ASC 에게 적용하기
	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		AbilitySystemComponent->MakeEffectContext() // Effect 가 어떤 Context 에서 생성됐는지 전달
		);

	// BP 함수는 BP 에서 구현되어있음 바로 호출
	BP_OnStoneConsumed();
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	// 오버랩 되는 경우 Hero Character 인 경우에 바로 PickUp Ability 활성화
	if (AWarriorHeroCharacter* OverlappedHeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		Debug::Print(TEXT("Overlap Start"));
		OverlappedHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGamePlayTags::Player_Ability_PickUp_Stones);
		if ( !PickUpWidgetComponent )
		{
			Debug::Print(TEXT("No PickUpWidgetComponent"));
			return;
		}
		PickUpWidgetComponent->SetVisibility(true);
		Debug::Print(TEXT("No PickUpWidgetComponent"));
		if ( UWarriorWidgetBase* InputWidget = Cast<UWarriorWidgetBase>(PickUpWidgetComponent->GetUserWidgetObject()) )
		{
			if (APlayerController* PC = Cast<APlayerController>(OverlappedHeroCharacter->GetController()))
				InputWidget->BP_OnSetInputKey(PC);
		}
	}
}

void AWarriorStoneBase::OnPickUpCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 오버랩 비활성 되는 경우 Hero Character 인 경우에 바로 PickUp Ability 활성화
	if (AWarriorHeroCharacter* OverlappedHeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		Debug::Print(TEXT("Overlap End"));
		FGameplayTagContainer TargetTags(WarriorGamePlayTags::Player_Ability_PickUp_Stones);
		OverlappedHeroCharacter->GetWarriorAbilitySystemComponent()->CancelAbilities(&TargetTags);
		if ( !PickUpWidgetComponent )
		{
			Debug::Print(TEXT("No PickUpWidgetComponent"));
			return;
		}
		PickUpWidgetComponent->SetVisibility(false);
	}
}
