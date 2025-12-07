// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

class UHeroUIComponent;
class UEnemyUIComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 위젯이 초기화 될 때 Player의 소유이면 HeroUIComponent 에 바인딩 될 수 있도록 함
	virtual void NativeOnInitialized() override;

	// 매개변수 UI Component 를 받아와서 위젯을 UI Component 에 바인딩, 구독하기 위한 함수
	UFUNCTION(BlueprintImplementableEvent,meta = (DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwningHeroUIComponent);
	
	UFUNCTION(BlueprintImplementableEvent,meta = (DisplayName = "On Owning Enemy UI Component Initialized"))
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OwningEnemyUIComponent);

public:
	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);
};
