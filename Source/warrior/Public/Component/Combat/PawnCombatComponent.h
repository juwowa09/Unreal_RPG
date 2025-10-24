// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Component/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"

#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};
/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	// 무기 관리 클래
	GENERATED_BODY()

public:
	// 새로 생성된 무기를 등록하는 함수 True 매개변수시 첫 무기를 장착까지 함
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	// 특정 무기 태그로 보유를 확인하는 함수
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	// 현재 장착중인 무기 태그
	UPROPERTY(BlueprintReadWrite, Category="Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	// 현재 장착중인 무기 태그 반환 함수
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	// 현재 무기의 Box Collision 활성, 비활성 토글함수
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void ToggleWeaponCollisioin(bool bShouldEnable, EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);

	// Weapon Delegate 에 등록할 콜백 함수
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

protected:
	TArray<AActor*> OverlappedActors;	// 중복처리 하지 않기 위한 배열 변수
	
private:
	// 지니고있는 무기를 태그별로 관리 
	TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponMap;
	
};
