// WonUngJu All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumTypes.h"

class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	FWarriorCountDownAction(float InTotalCountDownTime,float InUpdatedInterval,float& InOutRemainingTime,
		EWarriorCountDownActionOutput& InCountDownOutput, const FLatentActionInfo& LatentInfo)
			: bNeedToCancel(false),
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

	// 현재 액션 Cancel
	void CancelAction();
	
private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainingTime;
	EWarriorCountDownActionOutput& CountDownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};
