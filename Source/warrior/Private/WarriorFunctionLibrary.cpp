// WonUngJu All Rights Reserved


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);
	// 액터가 IAbilitySystemInterface 상속했으면 그 액터가 가지는 ASC 리턴, 아니면 nullptr 리턴
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
	/* CastChecked 는 실패시 Crash 발생
	 * Check 와 섞어놓음.
	 * 실제로 빌드 해버리면 Cast와 동일한 역할 Crash 발생 X */ 
}


void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		// 동일한 태그 여러번 추가 가능 내부적으로 Count +1
		// 그러나 지금 HasMatching 사용중이므로 단 하나만 됨
		ASC->AddLooseGameplayTag(TagToAdd);
		/* GiveAbility() 를 통해 어빌리티를 부여하는 순간,
		그 어빌리티에 정의된 “GameplayTags” 정보들이 ASC로 전달되어 저장(등록). */
	}
}

// Actor의 ASC 에서 태그 제거
void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		// 동일한 태그 여러번 제거 가능 내부적으로 Count -1
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

// C++ 버전으로 블루프린트에서 사용할 수 있도록 함수 구현
bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

// 결과를 블루프린트에서 out 할 수 있게 사용
void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	// 결과를 branch 로 나타내기 위해서 Enum 사용
	OutConfirmType = NativeDoesActorHaveTag(InActor,TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor)
	
	// 인터페이스 캐스팅, 인터페이스 구현되어있다면 // 해당 객체가 가지고있는 vtable 과 연결된 Interface 부분 포인터 반환
	// 객체에 알맞은 virtual 함수가 호출됨
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return CombatComponent;
}

