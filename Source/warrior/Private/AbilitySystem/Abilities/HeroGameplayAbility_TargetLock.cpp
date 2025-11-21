// WonUngJu All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Controllers/WarriorHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"

#include "WarriorDebugHelper.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	// 어빌리티 활성시 Target
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// 어빌리티 끝날 시 초기화
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// Tick Task 를 통해 계속해서 Widget 업데이트 하는 함수
void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, WarriorGamePlayTags::Shared_Status_Dead) ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGamePlayTags::Shared_Status_Dead)
		)
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotatioin = 
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGamePlayTags::Player_Status_Rolling);
	// && !UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGamePlayTags::Player_Status_Blocking);


	// 현재 캐릭터 방향 고정 회전값
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		CurrentLockedActor->GetActorLocation()
	);

	LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

	// 카메라는 Control Rot 을 사용함 오일러각
	const FRotator CurrentControlRot = GetHeroControllerFromActorInfo()->GetControlRotation();
	// 부드럽게 보간한 값을 가져옴
	const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot,LookAtRot,DeltaTime, TargetLockRotationInterpSpeed);

	GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch,TargetRot.Yaw,0.f));
	
	if (bShouldOverrideRotatioin)
	{
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f,TargetRot.Yaw,0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;
	
	GetAvailableActorsAroundTarget(ActorsOnLeft,ActorsOnRight);

	if (InSwitchDirectionTag == WarriorGamePlayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

// 실제로 Ability Activation 에서 호출, 그리는 함수
void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	// 비어있으면 Target 할 게 없으므로 리턴
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	// 가장가까운 녀석 가져오기
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		// 위젯 그리기
		DrawTargetLockWidget();

		// 위젯 위치 설정
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

// 타겟 락 가능한 적 받아오는 함수
void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	// 결과로 받아올 배열
	TArray<FHitResult> BoxTraceHits;

	// 현재 캐릭터 위치 + 앞방향으로 탐색, 박스를 start -> end 까지 sweep 하면서
	// BoxTrace Channel 안에 있는것들을 충돌했는지 검사
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(), // 시작위치, 끝 위치
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistancce,
		TraceBoxSize / 2.f,		// 박스의 크기 설정
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(), // 박스의 앞면 설정
		BoxTraceChannel,	// 감지할 타입 설정
		false,
		TArray<AActor*>(),		// 무시할 타입 설정 (세부 타입)
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,	// 디버깅 설정
		BoxTraceHits,	 // 결과 
		true		// 자신 무시
	);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		// 액터일 경우
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

// 현재 Hero 에서 가장 가까운 대상 가져오기
AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosetDistance = 0.f;

	// 가장 가까운 Target 하나 가져오기
	return UGameplayStatics::FindNearestActor(
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		InAvailableActors,
		ClosetDistance
	);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

// 실제로 Target Lock 그리는 부분
void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assgin a valid widget class in Blueprint"));

		// 위젯 create == controller 필
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		check(DrawnTargetLockWidget);

		// Widget Viewport 에 추가 (그리기)
		DrawnTargetLockWidget->AddToViewport();	
	}
}

// 뷰포트에 Lock Position 위치시키는 함수
void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;

	// Controller -> CameraManager -> Viewport -> CameraComponent 로 이어져서 Viewport 를 받아옴
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true		// 플레이어 Viewport 를 기준으로 그리기
	);

	// 타겟 위치를 왼쪽 모서리 기준으로 Widget 을 그렸기 때문에 반절씩 위치를 빼줘야함
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		// 람다 함수를 쓰는 이유 == 그 자리에서 즉석으로 처리할 작은 함수를 만들기, this 로 현재 객체 내부 변수 접근 가능
		// 내부 모든 위젯에 대해서 람다 함수 실행
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				// 찾은 것만 저장
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);		
	}
	// 제대로 위치 계산해서 적용
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	// PlayerController → LocalPlayer 로 연결 (인풋이 들어오는 곳이 LocalPlayer)
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	// 인풋 감진 서브시스템
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem)

	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

// 현재 어빌리티 하나만 캔슬하는 함수, true == 서버, 클라 모두에게 캔슬하라 명령
void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		true
		);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	// 어빌리티 끝날때 클린 업
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;

	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}
	// PlayerController → LocalPlayer 로 연결 (인풋이 들어오는 곳이 LocalPlayer)
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();

	// 인풋 감진 서브시스템
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem)

	Subsystem->RemoveMappingContext(TargetLockMappingContext);
}
