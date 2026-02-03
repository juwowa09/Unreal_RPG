// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/WarriorPickUpBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
class UInputAction;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorStoneBase : public AWarriorPickUpBase
{
	GENERATED_BODY()

public:
	// 돌 소비, 사라지는 함수
	void Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent,int32 ApplyLevel);
	
protected:
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnPickUpCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	// BP 에서 구현할 함수
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Stone Consumed"))
	void BP_OnStoneConsumed();
	
	// ASC 에 적용할 Effect Class
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
	
private:
	UInputAction* StoneInputAction;
};
