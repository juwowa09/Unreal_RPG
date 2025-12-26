// WonUngJu All Rights Reserved


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarriorGamePlayTags.h"
#include "WarriorTypes/WarriorCountDownAction.h"
#include "WarriorGameInstance.h"

#include "WarriorDebugHelper.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	checkf(InActor, TEXT("Null Actor"));
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

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	// 각도 계산하기 위한 벡터들
	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	// 피격자 기준으로 왼쪽에 Attacker 위치
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1;
	}
	if (-45 <= OutAngleDifference && OutAngleDifference <= 45)
		return WarriorGamePlayTags::Shared_Status_HitReact_Front;
	else if (-135 <= OutAngleDifference && OutAngleDifference < -45)
		return WarriorGamePlayTags::Shared_Status_HitReact_Left;
	else if (45 < OutAngleDifference && OutAngleDifference <= 135)
		return WarriorGamePlayTags::Shared_Status_HitReact_Right;
	else
		return WarriorGamePlayTags::Shared_Status_HitReact_Back;

	return FGameplayTag();
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	// 각도 차이
	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	// const FString DebugString = FString::Printf(TEXT("dot result: %f %s"), DotResult, DotResult< 0.f? TEXT("Valid") : TEXT("InValid"));
	// Debug::Print(DebugString,DotResult < -0.1f ? FColor::Green : FColor::Red);
	
	// 두 벡터가 서로 반대방향이라면 유효한 블럭 - 음수 (서로 마주보면)
	return DotResult < -0.1f;
}

// Instigator 가 존재하는 Actor가 Effect 적용시키는 함수
bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data,TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
	UPARAM(DisplayName="Output")EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;

	if (GEngine)
	{
		// 월드 받아오기
		World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
	}
	
	if (!World) return ;

	// 월드의 LatentManager 받아오기
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	// 우리가 찾을 Latent Class 할당해서 가져오기 (Callback Target, UUID)
	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		// 액션 시작하기
		if (!FoundAction)
		{
			// LatentManager 에 만들 Action 등록
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime,UpdateInterval,OutRemainingTime,CountDownOutput,LatentInfo)
			);
		}
	}
	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			// 액션 캔슬하기
			FoundAction->CancelAction();
		}
	}
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		// World Context 를 통해서 World 를 받아오기
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// World 의 GameInstance 를 받아오기
			return World->GetGameInstance<UWarriorGameInstance>();
		}
	}
	
	return nullptr;
}

void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;

	if (GEngine)
	{
		// 월드 받아와서 컨트롤러 할당하기
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
	}

	if (!PlayerController)
	{
		return;
	}

	// Input Game Mode 정의
	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;

	// Input 에 따라서 GameMode 를 변경하기
	switch (InInputMode)
	{
		// GameMode 로 설정, 마우스 커서 안보이게
	case EWarriorInputMode::GameOnly:
		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;
		break;
		
		// UIMode 로 설정, 마우스 커서 보이게
	case EWarriorInputMode::UIOnly:
		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;
		break;
	default:
		break;
	}
}

