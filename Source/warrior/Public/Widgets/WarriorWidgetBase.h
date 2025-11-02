// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

class UHeroUIComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 위젯 생성될 때 호출되는 함수
	virtual void NativeOnInitialized() override;

	// 매개변수 UI Component 에 구독하기 위한 함수
	UFUNCTION(BlueprintImplementableEvent,meta = (DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwningUIComponent);
};
