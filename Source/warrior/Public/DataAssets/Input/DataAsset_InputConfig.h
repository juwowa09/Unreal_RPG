// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()
public:
	//태그
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta = (Category="InputTag"))
	FGameplayTag InputTag;

	//실제 입력에 따른 액션
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputAction;
};
/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// Enhanced Input, 어떤 입력 -> 어떤 액션을 매핑하는 컨텍스트
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	// 여러개의 태그-액션 쌍을 저장
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta = (TitleProperty = "InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;
};
