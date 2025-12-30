// WonUngJu All Rights Reserved


#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Component/Input/WarriorInputComponent.h"
#include "WarriorGamePlayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Component/Combat/HeroCombatComponent.h"
#include "Component/UI/HeroUIComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameModes/WarriorBaseGameMode.h"

#include "WarriorDebugHelper.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	//기본 충돌 캡슐
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);

	//현재 컨트롤러로 캐릭터 회전 불가능
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 카메라 봉 생성자
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());	// 루트컴포넌트에 자식으로 붙이기
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.f,55.f,65.f);	// 자식의 offset 지정(카메라 오프셋)
	CameraBoom->bUsePawnControlRotation = true;		// 컨트롤러에 의해 회전이 되는지 설정

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// 이미 카메라봉이 회전하고 있으므로 회전 X

	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 wasd 이동시 회전여부
	GetCharacterMovement()->RotationRate = FRotator(0.f,500.f,0.f);	// 캐릭터 회전속도
	GetCharacterMovement()->MaxWalkSpeed = 400.f;	// 최대 걷기 속도
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;	// 이동 정지시 감속

	// CombatComponent 는 확장 컴포넌트 상속 (필요한 기능들 추가)
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

// 캐릭터가 컨트롤러에 연결될 때 호출되는 함수
void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// if (WarriorAbilitySystemComponent && WarriorAttributeSet)
	// {
	// 	const FString ASCText = FString::Printf(TEXT("Owner Actor: %s, Avatar Actor: %s"), *WarriorAbilitySystemComponent->GetOwnerActor()->GetActorLabel(), *WarriorAbilitySystemComponent->GetAvatarActor()->GetActorLabel());
	// 	Debug::Print(TEXT("Ability System Component valid.") + ASCText, FColor::Green);
	// 	Debug::Print(TEXT("Attribute System valid.") + ASCText,FColor::Green);
	// }
	if (!CharacterStartUpData.IsNull())
	{
		UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous();
		if (LoadedData)
		{
			int32 AbilityApplyLevel = 1;

			// GameMode 의 난이도를 통해 Ability Level 을 할당하는 로직
			if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
			{
				switch(BaseGameMode->GetCurrentGameDifficulty())
				{
				case EWarriorGameDifficulty::Easy:
					AbilityApplyLevel = 4;
					// Debug::Print(TEXT("Current Difficulty: Easy"));
					break;
				case EWarriorGameDifficulty::Normal:
					AbilityApplyLevel = 3;
					// Debug::Print(TEXT("Current Difficulty: Normal"));
					break;
				case EWarriorGameDifficulty::Hard:
					AbilityApplyLevel = 2;
					// Debug::Print(TEXT("Current Difficulty: Hard"));
					break;
				case EWarriorGameDifficulty::VeryHard:
					AbilityApplyLevel = 1;
					// Debug::Print(TEXT("Current Difficulty: Very Hard"));
				default:
					break;
				}
			}

			// 로드한 Ability 를 Ability Level 에 맞춰 ASC 에 모두 전달
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
		}
	}
}

// 컨트롤러 연결 후 인풋 바인딩 시점에 호출되는 함수
void AWarriorHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forgot to asign a vaild data asset as input config"))
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	
	check(SubSystem);
	SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	// 캐릭터 움직임 인풋 바인딩
	// 태그에 매핑된 인풋 액션을 찾아서 Call Back 함수의 매개변수로 넣어준다
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_Move,
		ETriggerEvent::Triggered,
		this,
		&ThisClass::Input_Move);

	// 카메라 움직임 인풋 바인딩(마우스)
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
	WarriorGamePlayTags::InputTag_Look,
	ETriggerEvent::Triggered,
	this,
	&ThisClass::Input_Look);

	// Target Lock Input Switch Bind
	// 태그에 매핑된 인풋 액션을 찾아서 Call Back 함수의 매개변수로 넣어준다
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_SwitchTarget,
		ETriggerEvent::Triggered,
		this,
		&ThisClass::Input_SwitchTargetTriggered);
	
	// 태그에 매핑된 인풋 액션을 찾아서 Call Back 함수의 매개변수로 넣어준다
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_SwitchTarget,
		ETriggerEvent::Completed,
		this,
		&ThisClass::Input_SwitchTargetCompleted);

	// 줍기 인풋과 줍는 함수 바인딩
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_PickUp_Stones,
		ETriggerEvent::Started,
		this,
		&ThisClass::Input_PickUpStonesStarted);

	// Data Asset에 등록된 어빌리티와 Input 바인딩
	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset,
		this,
		&ThisClass::Input_AbilityInputPressed,
		&ThisClass::Input_AbilityInputReleased);

	
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Debug::Print(TEXT("Working"));
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw,0.f);
	
	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	
	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw,0.f);
	
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	
	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void AWarriorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0.f ? WarriorGamePlayTags::Player_Event_SwitchTarget_Right : WarriorGamePlayTags::Player_Event_SwitchTarget_Left,
		Data
		);
	
	// Debug::Print(TEXT("SwitchDirection: ") + SwitchDirection.ToString());
}

// 줍기 함수 실행
void AWarriorHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		WarriorGamePlayTags::Player_Event_ConsumeStones,
		Data
		);
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	// 전달되는 태그는 Data Asset에 같이 매핑되어있는 태그
	
	// ASC 에 등록된 어빌리티와 동일한 태그를 가진 어빌리티 찾고 행위를 함 
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	// ASC 에 등록된 어빌리티와 동일한 태그를 가진 어빌리티 찾고 행위를 함
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}

