// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"

class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	FWarriorCountDownAction(float InTotalCountDownTime,float InUpdatedInterval,float& InOutRemainingTime,
		EWarriorCountDownActionOutput& InCountDownOutput, const FLatentActionInfo& LatentInfo)
			:
	bNeedToCancel(false),
	TotalCountDownTime(InTotalCountDownTime),
	UpdateInterval(InUpdatedInterval),
	OutRemainingTime(InOutRemainingTime),
	CountDownOutput(InCountDownOutput),
	ExecutionFunction(LatentInfo.ExecutionFunction),
	OutputLink(LatentInfo.Linkage),
	CallbackTarget(LatentInfo.CallbackTarget),
	ElapsedInterval(0.1),
	ElapsedTimeSinceStart(0.f)
	{}

	// Tick 을 통해 계속 Update 하는 함수
	virtual void UpdateOperation(FLatentResponse& Response) override;

	// 현재 액션 Cancel
	void CancelAction();
	
private:
	// Latent Cancel 여부
	bool bNeedToCancel;
	// 총 수행시간
	float TotalCountDownTime;
	// 어느 주기로 Update 하는지
	float UpdateInterval;
	// 얼마나 남았는지 계산
	float& OutRemainingTime;
	// Latent Action의 결과
	EWarriorCountDownActionOutput& CountDownOutput;
	// CallbackTarget 에서 호출할 함수 이름
	FName ExecutionFunction;
	// Output Pin 의 Index
	int32 OutputLink;
	// Latent Action 을 호출자 
	FWeakObjectPtr CallbackTarget;
	// 다음 주기까지 실행 시간
	float ElapsedInterval;
	// 총 수행한 시간
	float ElapsedTimeSinceStart;
};
