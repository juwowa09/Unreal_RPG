// WonUngJu All Rights Reserved


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"

#include "WarriorDebugHelper.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
                                                                               FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                               const FVector& SpawnOrigin, float RandomSpawnRadius)
{
	// Task 가 어떤 Ability 의 Task 인지 초기화
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	Node->CachedNumToSpawn = NumToSpawn;

	return Node;
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	// ASC 에 태그-델리게이트로 등록된 쌍을 가져오는 함수 (등록된 모든 콜백함수 목록)
	// GenericGameplayEventCallbacks - TMap 형태
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	// 델리게이트에 함수 삭제
	Delegate.Remove(DelegateHandle);
	// 마지막에 호출해야 정상 작동함
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	// ASC 에 태그-델리게이트로 등록된 쌍을 가져오는 함수 (등록된 모든 콜백함수 목록)
	// GenericGameplayEventCallbacks - TMap 형태
	// ASC에 등록한 Event Tag가 들어오면, GenericGameplayEventCallbacks 안에 등록된 콜백함수들을 모두 실행
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	// 델리게이트에 함수 추가, 나중에 삭제하기 위해 변수 저장
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		// 언리얼 엔진에서 에셋 관리를 담당하는 클래스
		// 비동기 로딩 디렉터리 path 로 요청
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			// 델리게이트를 생성 하고 콜백함수를 바인딩함 로드 완료된 순간 콜백함수 실행
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded));
	}
	else
	{
		// 현재 태스크가 델리게이트를 호출해도 되는지 확인
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	// 클래스 참조 가져오기
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();

	if (!LoadedClass || !World)
	{
		// 현재 태스크가 델리게이트를 호출해도 되는지 확인
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			// 스폰되지 않았음을 알림
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		EndTask();
		return;
	}
	
	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;

	// 스폰 액터의 충돌 처리방식 지정
	// 스폰 위치가 물체가 이미 있다면 다른 위치 지정, 결국 못찾아도 어디서든 스폰
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		// 네비게이션 안에서 랜덤위치 찾기, Params = 중심위치, 결과, 위치로부터 반경
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

		RandomLocation += FVector(0.f,0.f,150.f);

		const FRotator SpawnFacingRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();

		// 스폰 실행, 위치, 회전 값, 스폰 충돌 조건
		AWarriorEnemyCharacter* SpawnedEnemy = World->SpawnActor<AWarriorEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			// 스폰이 완료되었다는 것을 알림
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			// 스폰되지 않았음을 알림
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		
	}
	EndTask();
}
