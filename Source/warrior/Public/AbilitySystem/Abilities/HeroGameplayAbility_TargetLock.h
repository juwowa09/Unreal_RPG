// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UInputMappingContext;
class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

private:
	// Lock 실행하는 함수
	void TryLockOnTarget();
	// 박스 밀면서 Lock 가능한 Actor 찾는 함수
	void GetAvailableActorsToLock();
	// 가장 가까운 타겟 가져오는 함수
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	// 위젯 그리기
	void DrawTargetLockWidget();
	// Widget 위치 설정, 보정
	void SetTargetLockWidgetPosition();
	// 타겟 Lock 시 움직임 속도 설정
	void InitTargetLockMovement();
	// 매핑컨텍스트 추가
	void InitTargetLockMappingContext();

	// 어빌리티 끝날때 호출하는 함수들
	void CancelTargetLockAbility();
	void CleanUp();
	void ResetTargetLockMovement();
	void ResetTargetLockMappingContext();

	// 변수영역

	// 박스 거리 어디까지 탐색
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	float BoxTraceDistancce = 5000.f;

	// 박스 크기 설정
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	FVector TraceBoxSize = FVector(5000.f,5000.f,300.f);

	// 충돌 감지할 타입
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	TArray<TEnumAsByte<EObjectTypeQuery> > BoxTraceChannel;
	
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	bool bShowPersistentDebugShape;

	// 클래스 자체를 가리키는 변수
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	float TargetLockMaxWalkSpeed = 300.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Target Lock")
	UInputMappingContext* TargetLockMappingContext;
	
	// Lock 가능한 액터들 저장
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	UPROPERTY()
	AActor* CurrentLockedActor;
	
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;
	
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
	
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};
