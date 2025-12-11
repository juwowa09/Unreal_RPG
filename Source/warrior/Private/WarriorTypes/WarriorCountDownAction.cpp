// WonUngJu All Rights Reserved


#include "WarriorTypes/WarriorCountDownAction.h"

#include "WarriorDebugHelper.h"

void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	// Cancel 해야하는지 check
	if (bNeedToCancel)
	{
		// Output 에 Cancel 됐음을 알림
		CountDownOutput = EWarriorCountDownActionOutput::Cancelled;

		/* true = Latent Action 종
		 * Callback Target 에서 ExecutionFunction 실행,
		 * OutputLink 번호 연결
		 */
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		return;
	}

	// 실행시간이 넘어갔을 경우 Complete
	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		
		// Output 에 Cancel 됐음을 알림
		CountDownOutput = EWarriorCountDownActionOutput::Completed;

		/* true = Latent Action 종
		 * Callback Target 에서 ExecutionFunction 실행,
		 * OutputLink 번호 연결
		 */
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	// Interval 계속증가
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		// Update 주기마다 실행, 경과시간 계산
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();

		// 남은 시간 계산
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;

		// 현재 상태를 Output 에 연결
		CountDownOutput = EWarriorCountDownActionOutput::Updated;
		// 다음 트리거를 호출함 Output Link를
		Response.TriggerLink(ExecutionFunction,OutputLink,CallbackTarget);

		// 주기 다시 0으로 업데이트
		ElapsedInterval = 0;
	}
}

void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
