// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"

#include "WarriorFunctionLibrary.generated.h"


class UWarriorGameInstance;
struct FGameplayTag;
class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;
struct FScalableFloat;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Native 키워드 == C++ 레벨에서 정의된 함 / UFUNCTION 내부에서 호출용으로 씀
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	// ExpandEnumAsExecs == 지정한 Enum을 블루프린트에서 Exec 핀으로 확장(흰색 선)
	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType);

	// 두 액터가 같은 Team 인지 확인하는 헬퍼 함수
	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
	
	// 커브 테이블에서 Level 에 따른 Float 값 가져올 때 사용하는 함수
	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary", meta = (CompactNodeTitle="Get Value AT Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);

	// Attacker 와 피해자의 각도 차이 계산, 왼쪽 오른쪽 (외적으로 계산) 판별 함수
	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	// 두 액터가 서로 바라보고있는지 아닌지 판별
	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	// Effect Spec을 만들어서 Target에 적용하는 함수
	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	// LatentAction 을 만들 함수
	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta = (Latent, WorldContext="WorldContextObject", LatentInfo="LatentInfo",ExpandEnumAsExecs="CountDownInput|CountDownOutput",TotalTime="1.0",UpdateInterval="0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
		float& OutRemainingTime,EWarriorCountDownActionInput CountDownInput,
		UPARAM(DisplayName="Output")EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo);
	
	// GameInstance 를 받아오는 함수
	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary", meta=(WorldContext = "WorldContextObject"))
	static UWarriorGameInstance* GetWarriorGameInstance(const UObject* WorldContextObject);
};
